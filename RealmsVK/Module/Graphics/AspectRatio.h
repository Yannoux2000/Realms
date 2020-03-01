#pragma once
#include <memory>

namespace rlms{
	class AspectRatioImpl;

	class AspectRatio {
	private:
		static std::unique_ptr<AspectRatioImpl> instance;
	public:
		static void Set (unsigned int const& w = 800, unsigned int const& h = 600);

		static const unsigned int Width ();
		static const unsigned int Height ();

		static const float Ratio ();

		static const unsigned int RelativeWidth (float const& ratio);
		static const unsigned int RelativeHeight (float const& ratio);
	};
}