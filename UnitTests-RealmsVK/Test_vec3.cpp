#include "pch.h"

#include "Base/Math/Vec3.h"

using namespace rlms;
TEST (TestVec, DotProductNominal) {
	{
		SCOPED_TRACE ("int");
		Vec3<int> a ({ 1, 2, 3 });
		Vec3<int> b ({ -1,-2,-3 });

		int c = 0;
		ASSERT_NO_FATAL_FAILURE ({
			c = a.dot (b);
			});

		ASSERT_EQ (-14, c);
	}
	{
		SCOPED_TRACE ("char");
		Vec3<char> a ({ 1, 3,-5 });
		Vec3<char> b ({ 4,-2,-1 });

		char c = 0;
		ASSERT_NO_FATAL_FAILURE ({
			c = a.dot (b);
			});

		ASSERT_EQ (3, c);
	}
	{
		SCOPED_TRACE ("double");
		Vec3<double> a ({ 1.5, 2.7, -5.1 });
		Vec3<double> b ({ -9.9, 6.0, -2.0 });

		double c = 0;
		ASSERT_NO_FATAL_FAILURE ({
			c = a.dot (b);
			});

		ASSERT_NEAR (11.55, c, 0.01);
	}
}

TEST (TestVec, DotProduct) {
	{
		SCOPED_TRACE ("int");
		Vec3<int> a ({ 1, 2, 3 });
		Vec3<int> b ({ -1,-2,-3 });

		int c = 0;
		ASSERT_NO_FATAL_FAILURE ({
			c = a.dot (b);
			});

		ASSERT_EQ (-14, c);
	}
	{
		SCOPED_TRACE ("char");
		Vec3<char> a ({ 1, 3,-5 });
		Vec3<char> b ({ 4,-2,-1 });

		char c = 0;
		ASSERT_NO_FATAL_FAILURE ({
			c = a.dot (b);
			});

		ASSERT_EQ (3, c);
	}
	{
		SCOPED_TRACE ("double");
		Vec3<double> a ({ 1.5, 2.7, -5.1 });
		Vec3<double> b ({ -9.9, 6.0, -2.0 });

		double c = 0;
		ASSERT_NO_FATAL_FAILURE ({
			c = a.dot (b);
			});

		ASSERT_NEAR (11.55, c, 0.01);
	}
}
