#include <string>

namespace Shaders {

	const std::string cgwg_CRT_fragment = R"_cgwg_CRT_frag(
		#version 130

		in vec3 Color;
		out vec4 outColor;

		void main() {
	    	outColor = vec4(Color + 0.1, 1.0f);
		}
	)_cgwg_CRT_frag";

	const std::string cgwg_CRT_vertex = R"_cgwg_CRT_vert(
		#version 130

	    in vec2 position;
	    in vec3 color;
	    out vec3 Color;

	    void main() {
	        Color = color;
	        gl_Position = vec4(position, 0.0f, 1.0f);
	    }
	)_cgwg_CRT_vert";

}
