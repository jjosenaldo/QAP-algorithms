#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>


double calcVariancia ( std::vector<double> valores, double media )
{
	double var = 0;
	for (unsigned int i=0; i < valores.size(); i++)
		var += pow((valores[i]-media),2);
	return var/(valores.size()-1);
}

double calcMedia ( std::vector<double> valores )
{
	double media = 0;
	for (unsigned int i=0; i < valores.size(); i++)
		media += valores[i];
	return media/valores.size();
}

void calcResult ( std::vector<double> valores, std::string dado )
{
	double media = calcMedia(valores);
	double var = calcVariancia(valores, media);
	double dp = sqrt(var);

	std::cout << " | " << dado;
	std::cout << " | " << media;
	std::cout << " | " << var;
	std::cout << " | " << dp << std::endl; 
}

int main (int argc, char *argv[] )
{
	std::string file;
	
	if (argc == 2)
		file = argv[1];
	
	else
	{
		printf("%s\n", "ERROR - invalid argument\n");
		return 0;
	} 
	
	std::ifstream instanceFile (file);
	std::string instance;
	std::vector<double> tempo(4);
	std::vector<double> nonVisited(4);
	
	std::cout << "instancia | variável | média | variância | Desvio padrão\n";
	if (instanceFile.is_open())
	{
		std::string nome = "";
		double temp;

		while (!instanceFile.eof())
		{
			instanceFile >> nome;
			
			for (int i = 0; i < 4; i++)
			{
				instanceFile >> temp;
				tempo[i] = temp;
			}

			for (int i = 0; i < 4; i++)
			{
				instanceFile >> temp;
				nonVisited[i] = temp;
			}

			std::cout << nome;
			calcResult(tempo, "tempo");

			std::cout << nome;
			calcResult(nonVisited, "nós não visitados");

			for (int i =0; i < 4; i++)
			{
				tempo[i] = 0;
				nonVisited[i] = 0;
			}
		}

		instanceFile.close();
	}

	else 
	{
		std::cout << "Unable to open file " << file << std::endl;
		return 0;
	}

	return 0;
}