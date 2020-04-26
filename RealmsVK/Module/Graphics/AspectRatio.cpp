#include "AspectRatio.h"
#include <cmath>

using namespace rlms;

class rlms::AspectRatioImpl {
public:
	AspectRatioImpl (unsigned int const& w = 800, unsigned int const& h = 600) : m_width (w), m_height (h) {}
private:
	friend AspectRatio;

	unsigned int m_width;
	unsigned int m_height;

	const unsigned int width ();
	const unsigned int height ();

	const float ratio ();

	const unsigned int relativeWidth (float const& ratio);
	const unsigned int relativeHeight (float const& ratio);
};

const unsigned int AspectRatioImpl::width () {
	return m_width;
}
const unsigned int AspectRatioImpl::height () {
	return m_height;
}

const float AspectRatioImpl::ratio () {
	return (static_cast<float>(m_width)) / (static_cast<float>(m_height));
}

const unsigned int AspectRatioImpl::relativeWidth (float const& ratio) {
	return static_cast<unsigned int>(std::round (ratio * static_cast<float>(m_width)));
}
const unsigned int AspectRatioImpl::relativeHeight (float const& ratio) {
	return static_cast<unsigned int>(std::round (ratio * static_cast<float>(m_height)));
}

std::unique_ptr<AspectRatioImpl> AspectRatio::instance;
void AspectRatio::Set (unsigned int const& w, unsigned int const& h) {
	instance = std::make_unique<AspectRatioImpl> (w, h);
}

const unsigned int AspectRatio::Width () {
	return instance->width ();
}
const unsigned int AspectRatio::Height () {
	return instance->height ();
}

const float AspectRatio::Ratio () {
	return instance->ratio();
}

const unsigned int AspectRatio::RelativeWidth (float const& ratio) {
	return instance->relativeWidth (ratio);
}
const unsigned int AspectRatio::RelativeHeight (float const& ratio) {
	return instance->relativeHeight (ratio);
}
