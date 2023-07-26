#pragma once
namespace sdk {
	enum Version {
		// Format: major, minor, # of revision released
		V1_18_12 = 11804,// for example, 1.18.12 is the 4th revision of 1.18
		V1_19_41 = 11910,
		V1_19_51 = 11912,
		VLATEST = 99999,
	};

	extern int internalVers;
}
