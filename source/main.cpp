#include "pch.h"
//#undef main

//Standard includes
#include <iostream>

//Project includes
#include "ETimer.h"
#include "ERenderer.h"
#include <vector>
#include "Structs.h"
//#include "Triangle.h"
#include "Camera.h"
#include "SceneGraph.h"
#include "LightManager.h"
#include "DirectionalLight.h"


void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}
using namespace Elite;


void PrintKeys()
{
	std::cout << "hold left mouse = move left/right & forward/back\n";
	std::cout << "z = rest camerapos/rotation\n";
	std::cout << "c = next cullmode\n";
	std::cout << "f = next technique point->linear->anistropic \n";
	std::cout << "r = switch directX/rastirizer\n";
	std::cout << "0 = print keys\n";

}


int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;
	SDL_Window* pWindow = SDL_CreateWindow(
		"Raterizer & DirectX Merge - Brutin Bjorn",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Camera
	FVector3 cameraPos = FVector3(0.f, 0.f, -40.f);
	float FOV{ 60.f * float(M_PI) / 180.f };
	FOV = tanf(FOV / 2.f);
	float aspectRatio{ float(width) / float(height) };

	Camera* pCamera = new Camera(cameraPos, FOV, aspectRatio, height, width, 0.1f, 100.f);

	//SceneGraph
	SceneGraph* pScene = new SceneGraph{};
	

	LightManager* pLights = LightManager::GetInstance();
	pLights->AddLightToManager(new DirectionalLight(FVector3{ 0.577f, -0.577f, -0.577f }, RGBColor{ 1.f, 1.f, 1.f }, 1.5f));



	bool state = false;
	bool transparancy = true;
	//Initialize "framework"
	auto pTimer{ std::make_unique<Elite::Timer>() };
	auto pRenderer{ std::make_unique<Elite::Renderer>(pWindow,pScene,pCamera) };

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;

	//controls
	bool rightButton{ false };
	bool leftButton{ false };
	FPoint2 mousePos{};
	FPoint2 diffrence{};
	IPoint2 debug{};
	bool DepthVal{ false };

	PrintKeys();
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYDOWN:

				break;
			case SDL_KEYUP:
				if (e.key.keysym.scancode == SDL_SCANCODE_F)
				{
					for (size_t i = 0; i < pScene->GetShapeCount(); i++)
					{
						pScene->GetShape(i)->GetEffect()->NextTechnique();
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_T)
				{
					transparancy = !transparancy;
					std::cout << "toggle Transparancy: " << std::boolalpha << transparancy << std::endl;
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_R)
				{
					state = !state;
					std::cout << "is righthanded: " << std::boolalpha << state << std::endl;
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_C)
				{
					for (int i = 0; i < pScene->GetShapeCount(); i++)
					{
						pScene->GetShape(i)->NextCullMode();
						std::cout << "next cullmode" << std::endl;
					}
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_Z)
				{
					pCamera->ResetCamera();
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_0)
				{
					PrintKeys();
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				std::cout << "button pressed" << std::endl;
				if (e.button.button == SDL_BUTTON_LEFT)
				{
					std::cout << "left button" << std::endl;
					leftButton = true;
				}
				if (e.button.button == SDL_BUTTON_RIGHT)
				{
					std::cout << "right button" << std::endl;
					rightButton = true;
				}
				mousePos = FPoint2(float(e.button.x), float(e.button.y));
				diffrence = FPoint2(float(e.button.x), float(e.button.y));
				break;
			case SDL_MOUSEBUTTONUP:
				std::cout << "button released" << std::endl;
				if (e.button.button == SDL_BUTTON_LEFT)
				{
					std::cout << "left button" << std::endl;
					leftButton = false;
				}
				if (e.button.button == SDL_BUTTON_RIGHT)
				{
					std::cout << "right button" << std::endl;
					rightButton = false;
				}
				break;
			}
			mousePos = FPoint2(float(e.button.x), float(e.button.y));
			if (mousePos != diffrence)
			{
				FVector3 temp(mousePos - diffrence);

				if (leftButton && !rightButton)
				{
					pCamera->AddTranslation(FVector3(temp.x / 10.f, 0.f, -temp.y / 10.f));

				}
				if (!leftButton && rightButton)
				{
					pCamera->AddRotation({ temp.y / 180.f , temp.x / 180.f, 0.f });
				}
				if (leftButton && rightButton)
				{
					pCamera->AddTranslation({ 0.f,temp.y / 10.f,0.f });
				}
			}
			diffrence = mousePos;

		}

		//--------- Render ---------
		pRenderer->Render(state);


		//------- Rotate-------
		for (uint32_t i = 0; i < pScene->GetShapeCount(); i++)
		{
			pScene->GetShape(i)->AddRotation(30.f * float(M_PI) / 180.f * pTimer->GetElapsed());
		}

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		}

	}
	pTimer->Stop();

	//Shutdown "framework"

	ShutDown(pWindow);
	delete pScene;
	delete pCamera;
	return 0;
}