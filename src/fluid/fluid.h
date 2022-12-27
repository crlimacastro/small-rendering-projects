#pragma once
#include "../fae/fae.h"
// reference https://www.youtube.com/watch?v=alhpH6ECFvQ
struct fluid : public fae::application
{
	static const int N = 128;
	static const int SCALE = 4;
	static const int iter = 4;
	static const int dt = 0.1;

	struct Fluid
	{
		float diff = 0;
		float visc = 0;

		std::vector<float> s;
		std::vector<float> density;
		std::vector<float> Vx;
		std::vector<float> Vy;
		std::vector<float> Vx0;
		std::vector<float> Vy0;

		Fluid(float diff, float visc)
		{
			s.resize(N * N, 0);
			density.resize(N * N, 0);
			Vx.resize(N * N, 0);
			Vy.resize(N * N, 0);
			Vx0.resize(N * N, 0);
			Vy0.resize(N * N, 0);
		}

		int ix(int x, int y) {
			x = Clamp(x, 0, N - 1);
			y = Clamp(x, 0, N - 1);
			return x + y * N;
		}

		void addDensity(Vector2 i, float amount)
		{
			density[ix(i.x, i.y)] += amount;
		}

		void addVelocity(Vector2 i, Vector2 amount)
		{
			Vx[ix(i.x, i.y)] += amount.x;
			Vy[ix(i.x, i.y)] += amount.y;
		}

		void diffuse(int b, float x[], float x0[], float diff)
		{
			float a = dt * diff * (N - 2) * (N - 2);
			lin_solve(b, x, x0, a, 1 + 6 * a);
		}

		void lin_solve(int b, float x[], float x0[], float a, float c)
		{
			float cRecip = 1.0f / c;
			for (int k = 0; k < iter; k++)
			{
				for (int j = 1; j < N - 1; j++)
				{
					for (int i = 1; i < N - 1; i++)
					{
						x[ix(i, j)] = (x0[ix(i, j)] + a * (x[ix(i + 1, j)] + x[ix(i - 1, j)] + x[ix(i, j + 1)] + x[ix(i, j - 1)])) * cRecip;
					}
				}
			}

			set_bnd(b, x);
		}

		void project(float velocX[], float velocY[], float p[], float div[])
		{
			for (int j = 1; j < N - 1; j++)
			{
				for (int i = 1; i < N - 1; i++)
				{
					div[ix(i, j)] = -0.5f * (velocX[ix(i + 1, j)]
						- velocX[ix(i - 1, j)]
						+ velocY[ix(i, j + 1)]
						- velocY[ix(i, j - 1)]
						) / N;
					p[ix(i, j)] = 0;
				}
			}
			set_bnd(0, div);
			set_bnd(0, p);
			lin_solve(0, p, div, 1, 6);

			for (int j = 1; j < N - 1; j++)
			{
				for (int i = 1; i < N - 1; i++)
				{
					velocX[ix(i, j)] -= 0.5f * (p[ix(i + 1, j)] - p[ix(i - 1, j)]) * N;
					velocY[ix(i, j)] -= 0.5f * (p[ix(i, j + 1)] - p[ix(i, j - 1)]) * N;
				}
			}

			set_bnd(1, velocX);
			set_bnd(2, velocX);
		}

		void advect(int b, float d[], float d0[], float velocX[], float velocY[])
		{
			float i0, i1, j0, j1;
			float dtx = dt * (N - 2);
			float dty = dt * (N - 2);

			float s0, s1, t0, t1;
			float tmp1, tmp2, x, y;

			float Nfloat = N;
			float ifloat, jfloat;
			int i, j;

			for (j = 1, jfloat = 1; j < N - 1; j++, jfloat++)
			{
				for (i = 1, ifloat = 1; i < N - 1; i++, ifloat++)
				{
					tmp1 = dtx * velocX[ix(i, j)];
					tmp2 = dty * velocY[ix(i, j)];
					x = ifloat - tmp1;
					y = jfloat - tmp2;

					if (x < 0.5f) x = 0.5f;
					if (x > Nfloat + 0.5f) x = Nfloat + 0.5f;
					i0 = floorf(x);
					i1 = i0 + 1.0f;
					if (y < 0.5f) y = 0.5f;
					if (y > Nfloat + 0.5f) y = Nfloat + 0.5f;
					j0 = floorf(y);
					j1 = j0 + 1.0f;

					s1 = x - i0;
					s0 = 1.0f - s1;
					t1 = y - j0;
					t0 = 1.0f - t1;

					int i0i = i0;
					int i1i = i1;
					int j0i = j0;
					int j1i = j1;

					d[ix(i, j)] = s0 * (t0 * d0[ix(i0i, j0i)] + t1 * d0[ix(i0i, j1i)]) + s1 * (t0 * d0[ix(i1i, j0i)] + t1 * d0[ix(i1i, j1i)]);
				}
			}
		}

		void set_bnd(int b, float x[])
		{
			for (int i = 1; i < N - 1; i++)
			{
				x[ix(i, 0)] = b == 2 ? -x[ix(i, 1)] : x[ix(i, 1)];
				x[ix(i, N - 1)] = b == 2 ? -x[ix(i, N - 2)] : x[ix(i, N - 2)];
			}
			for (int j = 1; j < N - 1; j++)
			{
				x[ix(0, j)] = b == 1 ? -x[ix(1, j)] : x[ix(1, j)];
				x[ix(N - 1, j)] = b == 1 ? -x[ix(N - 2, j)] : x[ix(N - 2, j)];
			}

			x[ix(0, 0)] = 0.5 * (x[ix(1, 0)] + x[ix(0, 1)]);
			x[ix(0, N - 1)] = 0.5f * (x[ix(1, N - 1)] + x[ix(0, N - 2)]);
			x[ix(N - 1, 0)] = 0.5f * (x[ix(N - 2, 0)] + x[ix(N - 1, 1)]);
			x[ix(N - 1, N - 1)] = 0.5f * (x[ix(N - 2, N - 1)] + x[ix(N - 1, N - 2)]);
		}

		void step()
		{
			diffuse(1, Vx0.data(), Vx.data(), visc);
			diffuse(2, Vy0.data(), Vy.data(), visc);

			project(Vx0.data(), Vy0.data(), Vx.data(), Vy.data());

			advect(1, Vx.data(), Vx0.data(), Vx0.data(), Vy0.data());
			advect(2, Vy.data(), Vy0.data(), Vx0.data(), Vy0.data());

			project(Vx.data(), Vy.data(), Vx0.data(), Vy0.data());

			diffuse(0, s.data(), density.data(), diff);
			advect(0, density.data(), s.data(), Vx.data(), Vy.data());
		}

		void renderD()
		{
			for (int i = 0; i < N; i++)
			{
				for (int j = 0; j < N; j++)
				{
					float x = i * SCALE;
					float y = j * SCALE;
					float d = density[ix(i, j)];
					Color c = WHITE;
					c.a = d * 255;
					DrawRectangle(x, y, SCALE, SCALE, c);
				}
			}
		}
	};

	Fluid f = Fluid(0, 0);
	Vector2 prevMouse;

	void setup(fluid& app, entt::registry& reg)
	{
		auto& renderer = reg.ctx().at<fae::Renderer>();
		renderer.clearColor = BLACK;
	}


	void update(fluid& app, entt::registry& reg)
	{
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			app.f.addDensity({ (float)GetMouseX() / SCALE, (float)GetMouseY() / SCALE }, 100.f);
			float amtX = GetMouseX() - app.prevMouse.x;
			float amtY = GetMouseY() - app.prevMouse.y;
			app.f.addVelocity({ (float)GetMouseX() / SCALE, (float)GetMouseY() / SCALE }, { amtX, amtY });
		}
		app.f.step();
		app.prevMouse = GetMousePosition();
	}

	void draw(fluid& app, entt::registry& reg)
	{
		app.f.renderD();
	}

	fluid()
	{
		registry.ctx().emplace<fae::WindowDescriptor>("Euler Fluid Simulation");
		plugins.emplace(fae::rendering_plugin);
		systems.start.emplace<&fluid::setup>(*this);
		systems.update_controlled_gameobject.emplace < &fluid::update>(*this);
		systems.update_controlled_gameobject.emplace < &fluid::draw>(*this);
	}
};