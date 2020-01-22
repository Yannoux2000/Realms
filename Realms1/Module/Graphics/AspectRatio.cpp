#include "AspectRatio.h"

using namespace rlms;

class rlms::AspectRatioImpl {
public:
	AspectRatioImpl (unsigned int const& w = 800, unsigned int const& h = 600) : m_width (w), m_height (h) {}
private:
	friend rlms::AspectRatio;

	unsigned int m_width;
	unsigned int m_height;

	const unsigned int width ();
	const unsigned int height ();

	const float ratio ();

	const unsigned int relativeWidth (float const& ratio);
	const unsigned int relativeHeight (float const& ratio);
};

const unsigned int rlms::AspectRatioImpl::width () {
	return m_width;
}
const unsigned int rlms::AspectRatioImpl::height () {
	return m_height;
}

const float rlms::AspectRatioImpl::ratio () {
	return (static_cast<float>(m_width)) / (static_cast<float>(m_height));
}

const unsigned int rlms::AspectRatioImpl::relativeWidth (float const& ratio) {
	return (std::round (ratio * m_width));
}
const unsigned int rlms::AspectRatioImpl::relativeHeight (float const& ratio) {
	return (std::round (ratio * m_height));
}

std::unique_ptr<rlms::AspectRatioImpl> rlms::AspectRatio::instance;
void AspectRatio::Set (unsigned int const& w, unsigned int const& h) {
	instance = std::make_unique<AspectRatioImpl> (w, h);
}

const unsigned int AspectRatio::Width () {
	return instance->width ();
}
const unsigned int AspectRatio::Height () {
	return instance->height ();
}

const float rlms::AspectRatio::Ratio () {
	return instance->ratio();
}

const unsigned int AspectRatio::RelativeWidth (float const& ratio) {
	return instance->relativeWidth (ratio);
}
const unsigned int AspectRatio::RelativeHeight (float const& ratio) {
	return instance->relativeHeight (ratio);
}