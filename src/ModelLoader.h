#pragma once
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>

class ModelLoader
{
public:
	bool isLoad(const std::string& path);
	std::vector<std::vector<int>> fIndicesTrn;
	std::vector<glm::vec3> vCoordinates;
	std::vector<glm::vec2> vtCoordinates;
	std::vector<glm::vec3> vnCoordinates;
	std::vector<unsigned int> get_indicies();
private:
	bool parceVertices(const std::string& srVertex, glm::vec3& vertex);
	bool parceTextureCoords(const std::string& srTextureCoordinate, glm::vec2& textureCoordinate);
	bool parceNormal(const std::string& srNormal, glm::vec3& normal);
	bool parceFaces(std::string& srF, std::vector<int>& one_triangle);
};

inline bool ModelLoader::isLoad(const std::string& path){
    bool isLoad = true;
    std::fstream input_data(path);
    
    // Add detailed file opening check
    if (!input_data.is_open()) {
        std::cout << "ERROR: Cannot open file: " << path << std::endl;
        std::cout << "File might not exist or permission denied" << std::endl;
        return false;
    }

    std::string identifier;
    std::string parseData;
    int lineCount = 0;
    int vertexCount = 0, faceCount = 0;
    
    while (isLoad && input_data >> identifier)
    {
        lineCount++;
        std::getline(input_data, parseData);

        if (identifier == "v") {
            glm::vec3 v;
            if (parceVertices(parseData, v)) {
                vCoordinates.push_back(v);
                vertexCount++;
            }
            else {
                std::cout << "ERROR: Failed to parse vertex at line " << lineCount << std::endl;
                isLoad = false;
            }
        }
        else if (identifier == "vn") {
            glm::vec3 vn;
            if (parceNormal(parseData, vn))
                vnCoordinates.push_back(vn);
            else {
                std::cout << "ERROR: Failed to parse normal at line " << lineCount << std::endl;
                isLoad = false;
            }
        }
        else if (identifier == "vt") {
            glm::vec2 vt;
            if (parceTextureCoords(parseData, vt))
                vtCoordinates.push_back(vt);
            else {
                std::cout << "ERROR: Failed to parse texture coordinate at line " << lineCount << std::endl;
                isLoad = false;
            }
        }
        else if (identifier == "f") {
            std::vector<int> one_triangle;
            if (parceFaces(parseData, one_triangle)) {
                fIndicesTrn.push_back(one_triangle);
                faceCount++;
            }
            else {
                std::cout << "ERROR: Failed to parse face at line " << lineCount << std::endl;
                std::cout << "Face data: " << parseData << std::endl;
                isLoad = false;
            }
        }
    }
    
    input_data.close();
    
    //std::cout << "Loaded: " << vertexCount << " vertices, " << faceCount << " faces" << std::endl;
    
    if (vertexCount == 0 || faceCount == 0) {
        std::cout << "ERROR: No vertices or faces loaded - file might be empty or corrupt" << std::endl;
        isLoad = false;
    }
    
    return isLoad;
}

inline std::vector<unsigned int> ModelLoader::get_indicies()
{
	int N = fIndicesTrn.size();
	std::vector<unsigned int> indicies;
	int ind = 0;
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < 3; ++j) {
			indicies.push_back(fIndicesTrn[i].at(j*3));
		}
	}
	return indicies;
}

inline bool ModelLoader::parceVertices(const std::string& srVertex, glm::vec3& vertex)
{
	bool is_correct = true;
	std::stringstream ss(srVertex);
	double elem;
	std::vector<double> help_vec;
	while (ss >> elem) {
		help_vec.push_back(elem);
	}
	if (help_vec.size() == 3)
	{
		vertex.x = help_vec[0];
		vertex.y = help_vec[1];
		vertex.z = help_vec[2];
	}
	else
		is_correct = false;
	return is_correct;
}

inline bool ModelLoader::parceTextureCoords(const std::string& srTextureCoordinate, glm::vec2& textureCoordinate)
{
	bool is_correct = true;
	std::stringstream ss(srTextureCoordinate);
	double elem;
	std::vector<double> help_vec;
	while (ss >> elem) {
		help_vec.push_back(elem);
	}
	if (help_vec.size() == 2)
	{
		textureCoordinate.x = help_vec[0];
		textureCoordinate.y = help_vec[1];
	}
	else
		is_correct = false;
	return is_correct;
}

inline bool ModelLoader::parceNormal(const std::string& srNormal, glm::vec3& normal)
{
	bool is_correct = true;
	std::stringstream ss(srNormal);
	double elem;
	std::vector<double> help_vec;
	while (ss >> elem) {
		help_vec.push_back(elem);
	}
	if (help_vec.size() == 3)
	{
		normal.x = help_vec[0];
		normal.y = help_vec[1];
		normal.z = help_vec[2];
	}
	else
		is_correct = false;
	return is_correct;
}

inline bool ModelLoader::parceFaces(std::string& srF, std::vector<int>& one_triangle)
{
	std::replace(srF.begin(), srF.end(), '/', ' ');
	std::stringstream ss(srF);
	int elem;
	while (ss >> elem) {
		one_triangle.push_back(elem - 1);
	}
	return one_triangle.size() == 9;
}
