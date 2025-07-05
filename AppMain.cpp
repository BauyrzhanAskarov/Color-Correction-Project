//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"

#include "AppMain.h"
#include <iostream>

namespace ComputeOnDevice
{
	AppMain::AppMain(
		const std::shared_ptr<Graphics::DeviceResources>& deviceResources)
		: Holographic::AppMainBase(deviceResources)
		, _selectedHoloLensMediaFrameSourceGroupType(
			HoloLensForCV::MediaFrameSourceGroupType::PhotoVideoCamera)
		, _holoLensMediaFrameSourceGroupStarted(false)
		, _undistortMapsInitialized(false)
		, _isActiveRenderer(false)
	{

	}

	void AppMain::OnHolographicSpaceChanged(
		Windows::Graphics::Holographic::HolographicSpace^ holographicSpace)
	{
		//
		// Initialize the HoloLens media frame readers
		//
		StartHoloLensMediaFrameSourceGroup();
	}

	void AppMain::OnSpatialInput(
		_In_ Windows::UI::Input::Spatial::SpatialInteractionSourceState^ pointerState)
	{
		Windows::Perception::Spatial::SpatialCoordinateSystem^ currentCoordinateSystem =
			_spatialPerception->GetOriginFrameOfReference()->CoordinateSystem;

		if (!_isActiveRenderer)
		{
			std::cout << "Onspatial active" << std::endl;
			_currentSlateRenderer =
				std::make_shared<Rendering::SlateRenderer>(
					_deviceResources);
			_slateRendererList.push_back(_currentSlateRenderer);

			// When a Pressed gesture is detected, the sample hologram will be repositioned
			// two meters in front of the user.
			_currentSlateRenderer->PositionHologram(
				pointerState->TryGetPointerPose(currentCoordinateSystem));

			_isActiveRenderer = true;
		}
		else
		{
			// Freeze frame
			_visualizationTextureList.push_back(_currentVisualizationTexture);
			_currentVisualizationTexture = nullptr;
			_isActiveRenderer = false;
			std::cout << "Onspatial inactive" << std::endl;

		}
	}

	void AppMain::OnUpdate(
		_In_ Windows::Graphics::Holographic::HolographicFrame^ holographicFrame,
		_In_ const Graphics::StepTimer& stepTimer)
	{
		UNREFERENCED_PARAMETER(holographicFrame);

		dbg::TimerGuard timerGuard(
			L"AppMain::OnUpdate",
			30.0 /* minimum_time_elapsed_in_milliseconds */);

		//
		// Update scene objects.
		//
		// Put time-based updates here. By default this code will run once per frame,
		// but if you change the StepTimer to use a fixed time step this code will
		// run as many times as needed to get to the current step.
		//

		for (auto& r : _slateRendererList)
		{
			r->Update(
				stepTimer);
		}


		//
		// Process sensor data received through the HoloLensForCV component.
		//
		if (!_holoLensMediaFrameSourceGroupStarted)
		{
			return;
		}

		HoloLensForCV::SensorFrame^ latestFrame;

		latestFrame =
			_holoLensMediaFrameSourceGroup->GetLatestSensorFrame(
				HoloLensForCV::SensorType::PhotoVideo);

		if (nullptr == latestFrame)
		{
			return;
		}

		if (_latestSelectedCameraTimestamp.UniversalTime == latestFrame->Timestamp.UniversalTime)
		{
			return;
		}

		_latestSelectedCameraTimestamp = latestFrame->Timestamp;

		cv::Mat wrappedImage;
		std::cout << "Onsupdate" << std::endl;



		rmcv::WrapHoloLensSensorFrameWithCvMat(
			latestFrame,
			wrappedImage);

		// Define the combined matrix for protanopia simulation


		cv::Mat combinedMatrix2 = (cv::Mat_<double>(3, 3) <<
			0.112090805, 0.885306131, -0.000454506368,
			0.112653715, 0.889739721, -0.000110022065,
			0.0045248835, 0.0000820148395, 1.00191532
			);


		if (_undistortMapsInitialized) {
			cv::remap(wrappedImage, _undistortedPVCameraImage, _undistortMap1, _undistortMap2, cv::INTER_LINEAR);
			cv::resize(_undistortedPVCameraImage, _resizedPVCameraImage, cv::Size(), 0.5, 0.5, cv::INTER_AREA);
		}
		else {
			cv::resize(wrappedImage, _resizedPVCameraImage, cv::Size(), 0.5, 0.5, cv::INTER_AREA);
		}

		cv::medianBlur(_resizedPVCameraImage, _blurredPVCameraImage, 3);
		cv::cvtColor(_blurredPVCameraImage, _blurredPVCameraImage, cv::COLOR_BGRA2RGB);

		// Apply Tritanopia correction
		cv::Mat correctedImage;
		cv::transform(_blurredPVCameraImage, correctedImage, combinedMatrix2);

		// Step 2: Calculate the difference between original and corrected images
		cv::Mat img_diff = _blurredPVCameraImage - correctedImage;

		// Step 3: Shift colors towards visible spectrum (Tritanopia correction)
		// Define the error matrix for Tritanopia correction
		cv::Mat errorMatrix = (cv::Mat_<double>(3, 3) <<
			0, 0, 0,
			0.7, 1, 0,
			0.7, 0, 1,
			0, 0, 0
			);

		cv::Mat img_diff_corrected;
		cv::transform(img_diff, img_diff_corrected, errorMatrix);

		// Step 4: Add shifted colors to original image
		cv::Mat img_corrected = _blurredPVCameraImage + img_diff_corrected;

		cv::Mat transformedImager;
		cv::transform(img_corrected, transformedImager, combinedMatrix2);
		cv::cvtColor(transformedImager, transformedImager, cv::COLOR_RGB2BGRA);
		OpenCVHelpers::CreateOrUpdateTexture2D(_deviceResources, transformedImager, _currentVisualizationTexture);
	}

	void AppMain::OnPreRender()
	{
	}

	// Renders the current frame to each holographic camera, according to the
	// current application and spatial positioning state.
	void AppMain::OnRender()
	{
		std::cout << "Onrener" << std::endl;

		// Draw the sample hologram.
		for (size_t i = 0; i < _visualizationTextureList.size(); ++i)
		{
			_slateRendererList[i]->Render(
				_visualizationTextureList[i]);
		}

		if (_isActiveRenderer)
		{
			_currentSlateRenderer->Render(_currentVisualizationTexture);
		}
	}

	// Notifies classes that use Direct3D device resources that the device resources
	// need to be released before this method returns.
	void AppMain::OnDeviceLost()
	{

		for (auto& r : _slateRendererList)
		{
			r->ReleaseDeviceDependentResources();
		}

		_holoLensMediaFrameSourceGroup = nullptr;
		_holoLensMediaFrameSourceGroupStarted = false;

		for (auto& v : _visualizationTextureList)
		{
			v.reset();
		}
		_currentVisualizationTexture.reset();
	}

	// Notifies classes that use Direct3D device resources that the device resources
	// may now be recreated.
	void AppMain::OnDeviceRestored()
	{
		for (auto& r : _slateRendererList)
		{
			r->CreateDeviceDependentResources();
		}

		StartHoloLensMediaFrameSourceGroup();
	}

	void AppMain::StartHoloLensMediaFrameSourceGroup()
	{
		_sensorFrameStreamer =
			ref new HoloLensForCV::SensorFrameStreamer();

		_sensorFrameStreamer->EnableAll();

		_holoLensMediaFrameSourceGroup =
			ref new HoloLensForCV::MediaFrameSourceGroup(
				_selectedHoloLensMediaFrameSourceGroupType,
				_spatialPerception,
				_sensorFrameStreamer);

		_holoLensMediaFrameSourceGroup->Enable(
			HoloLensForCV::SensorType::PhotoVideo);

		concurrency::create_task(_holoLensMediaFrameSourceGroup->StartAsync()).then(
			[&]()
		{
			_holoLensMediaFrameSourceGroupStarted = true;
		});
	}
}