#pragma once
namespace SDK {
	enum Version {
		// Format: major, minor, # of revision released
		V1_16_40 = 11605,
		V1_18_12 = 11804,// for example, 1.18.12 is the 4th revision of 1.18
		V1_19_41 = 11910,
		V1_19_51 = 11912,
		V1_20 = 12000,
		V1_20_15 = 12005,
		V1_20_30 = 12006,
		V1_20_40 = 12009,
		V1_20_50 = 12011,
		VLATEST = V1_20_50,
	};

	extern int internalVers;
}
