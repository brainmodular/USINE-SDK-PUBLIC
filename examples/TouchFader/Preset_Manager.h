#ifndef __PRESET_MANAGER_H__
#define __PRESET_MANAGER_H__

#include "../../sdk/UserDefinitions.h" 
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class Preset
{

public:
	Preset();
	Preset(string Path);
	
	void Store_Single(string tab ,float* address, string name, float min, float max, TScale scale, AnsiCharPtr symbol, AnsiCharPtr format, float defaultValue);
	void Store_Boolean(string tab ,LongBool* address, string name);
	void Store_Integer(string tab ,int* address, string name, int min, int max, TScale scale, AnsiCharPtr symbol, int defaultValue);
	void Store_Combobox(string tab, int* address, string name, AnsiCharPtr commaText);
	void Store_Caption(string tab, AnsiCharPtr caption);
	void Store_Color(string tab, TUsineColor* address, string name);

	void SaveToFile(string Path);
	void ReadFromFile(string Path);


private:
	vector<pair<float*, string> > FPresets;
	vector<pair<LongBool*, string> > BPresets;
	vector<pair<int*, string> > IPresets;
	vector<pair<TUsineColor*, string> > CPresets;
};




#endif //__PRESET_MANAGER_H__
