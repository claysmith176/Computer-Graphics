#pragma once
class perlin {
public:
	perlin() {
		ranvec = new vec3[point_count];
		for (int i = 0; i < point_count; i++) {
			ranvec[i] = random_unit_vector();
		}
		perm_x = perlin_generate_perm();
		perm_y = perlin_generate_perm();
		perm_z = perlin_generate_perm();
	}
	~perlin() {
		delete[] ranvec;
		delete[] perm_x;
		delete[] perm_y;
		delete[] perm_z;
	}
	float noise(const vec3& p) const {
		auto u = fade(p.x() - floor(p.x()));
		auto v = fade(p.y() - floor(p.y()));
		auto w = fade(p.z() - floor(p.z()));

		int i = static_cast<int>(floor(p.x())) & 255;
		int j = static_cast<int>(floor(p.y())) & 255;
		int k = static_cast<int>(floor(p.z())) & 255;
		vec3 c[2][2][2];
		for (int di = 0; di < 2; di++) {
			for (int dj = 0; dj < 2; dj++) {
				for (int dk = 0; dk < 2; dk++) {
					c[di][dj][dk] = ranvec[
							perm_x[(i + di) & 255] ^
							perm_y[(j + dj) & 255] ^
							perm_z[(k + dk) & 255]];
				}
			}
		}
		return tlerp(c, u, v, w);
		//return ranfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
	}
	float turb(const point3& p, int depth = 7) const {
		auto accum = 0.0;
		auto temp_p = p;
		auto weight = 1.0;

		for (int i = 0; i < depth; i++) {
			accum += weight * noise(temp_p);
			weight *= 0.5;
			temp_p *= 2;
		}

		return fabs(accum);
	}


private:
	static const int point_count = 256;
	vec3* ranvec;
	int* perm_x;
	int* perm_y;
	int* perm_z;

	static float fade(float t) {
		return t * t * t * (t * (t * 6 - 15) + 10);
	}
	static float tlerp(vec3 c[2][2][2], float u, float v, float w) {
		float acum = 0;
		for (int k = 0; k < 2; k++) {
			for (int j = 0; j < 2; j++) {
				for (int i = 0; i < 2; i++) {
					vec3 weight(u - i, v - j, w - k);
					acum += (i * u + (1 - i) * (1 - u)) *
						(j * v + (1 - j) * (1 - v)) *
						(k * w + (1 - k) * (1 - w)) * 
						dot(c[i][j][k], weight);
				}
			}
		}
		return acum;
	}
	static int* perlin_generate_perm() {
		auto p = new int[point_count];
		for (int i = 0; i < point_count; i++) {
			p[i] = i;
		}
		permute(p, point_count);
		return p;
	}
	static void permute(int* p, int n) {
		for (int i = n - 1; i > 0; i--) {
			int target = random_int(0, i);
			int tmp = p[i];
			p[i] = p[target];
			p[target] = tmp;
		}
	}
};
