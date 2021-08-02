#pragma once
class Texture {
public:
	virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class Solid_Texture : public Texture {
public:
	Solid_Texture(vec3 c) : color(c) {}
	Solid_Texture(int r, int g, int b) : color(vec3(r, g, b)) {}

	vec3 value(float u, float v, const vec3& p) const override {
		return color;
	}
private:
	vec3 color;
};

class Checkered_Texture : public Texture {
public:
	Checkered_Texture(vec3 c1, vec3 c2) 
		: color1(std::make_shared<Solid_Texture>(c1)), color2(std::make_shared<Solid_Texture>(c2)) {}
	Checkered_Texture(std::shared_ptr<Texture> c1, std::shared_ptr<Texture> c2) : color1(c1), color2(c2) {}

	vec3 value(float u, float v, const vec3& p) const override {
		float scale = 100;
		float test = fmod(u * scale * 2, 1);
		float test2 = fmod(v * scale, 1);
		if (test <= 0.5 ^ test2 <= 0.5) {
			return color1->value(u, v, p);
		}
		else {
			return color2->value(u, v, p);
		}
	}
private:
	std::shared_ptr<Texture> color1;
	std::shared_ptr<Texture> color2;
};

class Noise_Texture : public Texture {
public:
	Noise_Texture() { scale = 1; }
	Noise_Texture(float s) : scale(s) {}

	vec3 value(float u, float v, const vec3& p) const override {
		return vec3(1, 1, 1) * 0.5 * (1 + sin((scale * p.z()) +10*noise.turb(p)));
	}
private:
	perlin noise;
	float scale;
};

class Image_Texture : public Texture {
public:
	Image_Texture() {}
	Image_Texture(std::string filename) {
		int compoents_thing = 0;
		data = stbi_load(&filename[0], &width, &height, &compoents_thing, bytes_per_pizel);
		if (!data) {
			std::cerr << "Image Failed to Load" << std::endl;
			width = height = 0;
		}
		bytes_per_scanline = bytes_per_pizel * width;
	}
	vec3 value(float u, float v, const vec3& p) const override {
		if (data == nullptr) { return vec3(0, 1, 1); }

		u = 1 - u;

		int i = static_cast<int>(u * width);
		int j = static_cast<int>(v * height);
		if (i >= width) { i = width - 1; }
		if (j >= height) { j = height - 1; }
		
		float color_scale = 1.0f / 255.0f;
		unsigned char* pixel = data + bytes_per_scanline * j + bytes_per_pizel * i;
		return color_scale * vec3(pixel[0], pixel[1], pixel[2]);
	}

private:
	const static int bytes_per_pizel = 3;
	unsigned  char* data;
	int width, height;
	int bytes_per_scanline;
};