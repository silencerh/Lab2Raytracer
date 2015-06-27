#include <iostream>
#include <glm/glm.hpp>
#include "SDL.h"
#include "SDLauxiliary.h"
#include "TestModel.h"
#include <math.h>
#include <algorithm>


using namespace std;
using glm::vec3;
using glm::mat3;

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;
vector<Triangle> triangles;
struct Intersection
{
	vec3 position;
	float distance;
	int triangleIndex;
};
Intersection closestIntersection;
vec3 cameraPos(0, 0, -2);
float yaw=0.0f*3.1415926/180;
vec3 lightPos(0,-0.5,-0.8);
vec3 lightColor = 6.1f * vec3(1,1,1);
vec3 indirectLight = 0.5f*vec3(1, 1, 1);
vec3 black(0, 0, 0);
vec3 white(1, 1, 1);
// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();
bool ClosestIntersection(
	vec3 start,
	vec3 dir,
	const vector<Triangle>& triangles,
	Intersection& closestIntersection
	);
vec3 DirectLight(const Intersection& i);


int main( int argc, char* argv[] )
{
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();	// Set start value for timer.

	while( NoQuitMessageSDL() )
	{
		Update();
		Draw();
	}

	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;
	Uint8* keystate = SDL_GetKeyState(0);
	if (keystate[SDLK_UP])
	{
		// Move camera forward
		cameraPos.z += 0.1f;
	}
	if (keystate[SDLK_DOWN])
	{
		// Move camera backward
		cameraPos.z -= 0.1f;
	}
	if (keystate[SDLK_LEFT])
	{
		// Rotate the camera along the y axis
		yaw += 0.1f;
	}
	if (keystate[SDLK_RIGHT])
	{
		// Rotate the camera along the y axis
		yaw -= 0.1f;
	}
	if (keystate[SDLK_w])
	{
		// Move lightsource forward

		lightPos.z += 0.1f;
	}
	if (keystate[SDLK_s])
	{
		// Move lightsource backward
		lightPos.z -= 0.1f;
	}
	if (keystate[SDLK_a])
	{
		// Move lightsource left
		lightPos.x -= 0.1f;
	}
	if (keystate[SDLK_d])
	{
		// Move lightsource right
		lightPos.x += 0.1f;
	}
	if (keystate[SDLK_q])
	{
		// Move lightsource up
		lightPos.y -= 0.1f;
	}
	if (keystate[SDLK_e])
	{
		// Move lightsource down
		lightPos.y += 0.1f;
	}

}

void Draw()
{
	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);
	//load model
	LoadTestModel(triangles);

	vec3 dir,color;

	mat3 R(cos(yaw),0,sin(yaw),0,1,0,-sin(yaw),0,cos(yaw));

	float f = SCREEN_HEIGHT / 2;
	
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; ++x)
		{
			vec3 dir(x - SCREEN_HEIGHT / 2, y - SCREEN_WIDTH/2, f);
			
			dir = R*dir;
			if (ClosestIntersection(cameraPos, dir, triangles, closestIntersection))
			{
				vec3 directLight = DirectLight(closestIntersection);
				vec3 ro = indirectLight; //+	directLight;
				color = ro*triangles[closestIntersection.triangleIndex].color;
					PutPixelSDL(screen, x, y, directLight);
			}
			else{ PutPixelSDL(screen, x, y, black); }

		}
	}

	

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}
bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection)
{
	vec3 e1,e2,b,result,e;
	float distance_temp;
	bool flag=false;
	closestIntersection.distance = std::numeric_limits<float>::max();
	int size = triangles.size();
	for( int i = 0;i < size; ++i )
	{
		e1 = triangles[i].v1-triangles[i].v0;
		e2 = triangles[i].v2-triangles[i].v0;
		b = start - triangles[i].v0;
		mat3 A(-dir, e1, e2);

		e = glm::inverse(A)*b;
		result = e.y*e1 + e.z*e2+triangles[i].v0;
		
		if ((e.x >= 0.0f)&&(e.y >= 0.0f) && (e.z >= 0.0f) && (e.y + e.z <= 1.0f))
		{		
				flag = true;
				distance_temp = glm::length(result - start);
				if (distance_temp < closestIntersection.distance)
				{
					closestIntersection.distance = distance_temp;
					closestIntersection.position = result;
					closestIntersection.triangleIndex = i;
				}			
		}
	}
	return flag;
}

vec3 DirectLight(const Intersection& i)
{
	float pi = 3.1415926f;
	vec3 directLight(0, 0, 0);
	Intersection cIntersection;
	vec3 direction = lightPos - i.position;
	ClosestIntersection(i.position, direction, triangles, cIntersection);
	if (cIntersection.distance >= glm::length(direction))
	{
		float area =  4 * pi*glm::length(direction)*glm::length(direction);
		float project_surface =(float) fmax(glm::dot(direction, triangles[i.triangleIndex].normal), 0);
		directLight =  lightColor*project_surface / area;
	}
	return directLight;
}
