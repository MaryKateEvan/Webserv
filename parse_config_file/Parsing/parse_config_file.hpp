
#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "../Stringing/StringHelp.hpp"
#include "../Stringing/StringArr.hpp"
#include "../Stringing/Pair.hpp"
#include "../Stringing/Twin.hpp"
#include "../Stringing/StringDataTracker.hpp"
#include "ConfigParse.hpp"
#include "ConfigData.hpp"

bool	read_config_file(std::string file);
