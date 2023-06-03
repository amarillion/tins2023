#pragma once

#include <functional>
#include <memory>

// forward declarations;
class Resources;
struct ALLEGRO_SHADER;
struct ALLEGRO_BITMAP;

/**
 * Wrapper for a time-dependent shader
 * that produces a dissolve effect
 */
class IrisEffect {

private:
	ALLEGRO_SHADER *shader;
	static std::string fragShaderSource;
	static std::string vertShaderSource;
	static bool inited;
	static ALLEGRO_BITMAP *gradient;
public:
	static void init(std::shared_ptr<Resources> res);
	IrisEffect();
	void enable(float time, int ofstx, int ofsty);
	void disable();
	ALLEGRO_SHADER *getShader() { return shader; }
};
