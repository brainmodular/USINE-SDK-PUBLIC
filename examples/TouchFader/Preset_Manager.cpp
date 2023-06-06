#include "Preset_Manager.h"


Preset::Preset()
{

}

Preset::Preset(string Path) //todo
{

}

void Preset::Store_Single(string tab, float* address, string name, float min, float max, TScale scale, AnsiCharPtr symbol, AnsiCharPtr format, float defaultValue)
{
	FPresets.push_back(make_pair(address, name));
}
void  Preset::Store_Boolean(string tab,LongBool* address, string name)
{
	BPresets.push_back(make_pair(address ,name));
}
void Preset::Store_Integer(string tab,int* address , string name, int min, int max, TScale scale, AnsiCharPtr symbol, int defaultValue)
{
	IPresets.push_back(make_pair(address, name ));
}
void Preset::Store_Combobox(string tab, int* address, string name, AnsiCharPtr commaText)
{
	IPresets.push_back(make_pair(address, name));
}
void Preset::Store_Caption(string tab, AnsiCharPtr caption)
{}
void Preset::Store_Color(string tab, TUsineColor* address, string name)
{
	CPresets.push_back(make_pair(address, name));
}


void  Preset::SaveToFile(string Path)
{
	//fstream outfile;
	ofstream outfile;
	outfile.open(Path);

	for (vector<pair<float *, string> >::const_iterator it = FPresets.begin(); it != FPresets.end(); it++)
	{
		outfile << "F"   << endl;
		outfile <<  it->second << endl;
		outfile << *it->first << endl;
	}
	for (vector<pair<LongBool *, string> >::const_iterator it = BPresets.begin(); it != BPresets.end(); it++)
	{
		outfile << "B" << endl;
		outfile << it->second << endl;
		outfile << *it->first << endl;
	}
	for (vector<pair<int *, string> >::const_iterator it = IPresets.begin(); it != IPresets.end(); it++)
	{
		outfile << "I"<< endl;
		outfile << it->second << endl;
		outfile << *it->first << endl;
	}
	for (vector<pair<TUsineColor *, string> >::const_iterator it = CPresets.begin(); it != CPresets.end(); it++)
	{
		outfile << "C" << endl;
		outfile << it->second << endl;
		outfile << *it->first << endl;
	}

	outfile.close();
}


void Preset::ReadFromFile(string Path)
{
	fstream infile;
	infile.open(Path);

	string line;
	float F_value = 0;
	int I_value = 0;
	LongBool LongBool_value;
	string name;
	TUsineColor C_value;

	//vector<pair<float *, string>>::iterator fit = FPresets.begin();
	//vector<pair<LongBool *, string>>::iterator bit = BPresets.begin();
	//vector<pair<int *, string>>::iterator iit = IPresets.begin();

	while (getline(infile, line))
	{
		if (line == "F")
		{
			getline(infile, line);
			name = line;         //next is name of variable to find
			getline(infile, line); //next is value
			F_value = (float)atof(line.c_str()); //string to F value

			//fit = find(FPresets.begin(), FPresets.end(), name);

			for (size_t i = 0; i < FPresets.size(); i++)  //loop in vector pair to find name
			{
				if (FPresets.at(i).second == name)  //if name is found 
				{
					*FPresets.at(i).first = F_value;   // then pass value to pointer
					break;
				}

			}
			//fit->second = atof(line.c_str());
			//fit++;
		}
		else if (line == "B")
		{
			getline(infile, line);
			name = line;         //next is name of variable to find
			getline(infile, line); //nexix is val
			if (line == "0"){LongBool_value = FALSE;}else { LongBool_value = TRUE; }

			for (size_t i = 0; i < BPresets.size(); i++)  //loop in vector pair to find name
			{
				if (BPresets.at(i).second == name)  //if name is found 
				{
					*BPresets.at(i).first = LongBool_value;   // then pass value to pointer
					break;
				}

			}

			//bit->second = LongBool_value;
			//bit++;
		}
		else if (line == "I")
		{
			getline(infile, line);
			name = line;         //next is name of variable to find
			getline(infile, line); //next is value
			I_value = (int)atof(line.c_str()); //string to F value

										  //fit = find(FPresets.begin(), FPresets.end(), name);

			for (size_t i = 0; i < IPresets.size(); i++)  //loop in vector pair to find name
			{
				if (IPresets.at(i).second == name)  //if name is found 
				{
					*IPresets.at(i).first = I_value;   // then pass value to pointer
					break;
				}

			}

		}
		else if (line == "C")
		{
			getline(infile, line);
			name = line;         //next is name of variable to find
			getline(infile, line); //next is value
			C_value = (int)atof(line.c_str()); //string to F value

										  //fit = find(FPresets.begin(), FPresets.end(), name);

			for (size_t i = 0; i < CPresets.size(); i++)  //loop in vector pair to find name
			{
				if (CPresets.at(i).second == name)  //if name is found 
				{
					*CPresets.at(i).first = C_value;   // then pass value to pointer
					break;
				}

			}

		}

		
	}


	infile.close();

}


