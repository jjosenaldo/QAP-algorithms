#include "latex.h"


std::string montar_cabecalho ()
{
	std::string cab;

	cab = "\\begin{table}[H]\n";
    cab += "\\centering\n";
    cab += "\\begin{tabular}{c|c|c|c|c|c|c|c|} \\hline \\hline\n";
    cab += "INST & N & AET(ms) & POSF(\\%) & DMinFC(\\%) & DMaxFC(\\%) & DAFC(\\%) & SDC \\\\ \\hline \\hline\n";

    return cab;
}

std::string montar_fim ()
{
	std::string fim;
	fim = "\\end{tabular}\n";
    fim += "\\caption{}\n";
    fim += "\\label{tab:}\n";
	fim += "\\end{table}\n";

	return fim;
}

void criar_tabelas_latex ()
{
	std::ifstream file ("archives/in_latex.dat");
	std::ofstream latex("archives/out_latex.dat", std::ofstream::app);
	if (!latex.is_open()) std::cout << "\nNão abriu o arquivo!\n";

	std::string linha;
	std::string aux;

	latex << montar_cabecalho ();

	if (file.is_open())
	{
		while (!file.eof())
		{
			for (int i=0; i < 8; i++)
			{
				file >> aux;
				linha += aux;
				if ( i != 7) linha += " & ";
				else linha += " \\\\ ";
			}
			latex << linha << std::endl;
			linha = "";
		}
		latex << montar_fim ();
		file.close();
		latex.close();
	}

	else 
	{
		std::cout << "ERROR - archive latex.cpp\n";
		std::cout << "Unable to open file archives/in_latex.dat " << std::endl;
	}
}
