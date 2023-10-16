#include <Windows.h>
#include <wininet.h>
#include <string>
#include <iostream>
#include <tchar.h>
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"wininet.lib")

using namespace std;

class cFuncoes
{
private:

	HINTERNET Solicitar;
	HINTERNET Autenticar;

public:

	bool InicializarSolicitacaoFTP(wstring Servidor, wstring Usuario, wstring Senha)
	{
		bool bRetorno = false;

		Solicitar = InternetOpen(L"ConsoleApplication1", INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0);
		if (Solicitar == NULL)
		{
			cout << "Ocorreu um erro durante a operação..\n" << GetLastError();
		}
		else
		{
			Autenticar = InternetConnect(Solicitar, Servidor.c_str(), INTERNET_DEFAULT_FTP_PORT, Usuario.c_str(),
				Senha.c_str(), INTERNET_SERVICE_FTP, INTERNET_FLAG_HYPERLINK | INTERNET_FLAG_TRANSFER_BINARY, 0);
			if (Autenticar == NULL)
			{
				cout << "Ocorreu um erro ao efetuar a autenticação..\n" << GetLastError();
			}
			else
			{
				bRetorno = true;
			}
		}

		return bRetorno;
	}

	void CriarDiretorio(wstring Diretorio, bool Remover)
	{
		if (Remover == true)
		{
			FtpRemoveDirectory(Autenticar, Diretorio.c_str());
		}
		else
		{
			FtpCreateDirectory(Autenticar, Diretorio.c_str());
		}
	}

	void EnviarArquivo(wstring ArquivoLocal, wstring LocalRemoto)
	{
		/*
		* Macro FTP_TRANSFER_TYPE_BINARY irá efetuar a transferência mantendo as propriedades originais do arquivo.
		*/
		FtpPutFile(Autenticar, ArquivoLocal.c_str(), LocalRemoto.c_str(), FTP_TRANSFER_TYPE_BINARY, 0);
	}

	void DeletarArquivo(wstring Arquivo)
	{
		FtpDeleteFile(Autenticar, Arquivo.c_str());
	}

	void ListarArquivos(wstring Diretorio)
	{
		WIN32_FIND_DATA Arquivo;

		int Quantidade = 0;

		HINTERNET First = FtpFindFirstFile(Autenticar, Diretorio.c_str(), &Arquivo, INTERNET_FLAG_HYPERLINK, 0);
		do
		{
			Quantidade += 1;
			_tprintf(L"%s\n", Arquivo.cFileName);

			long Tamanho = (double)(Arquivo.nFileSizeHigh * (MAXDWORD + 1)) + Arquivo.nFileSizeLow;
			cout << "Tamanho: " << Tamanho << " bytes\n";

		} while (InternetFindNextFile(First, &Arquivo));

		cout << "\nQuantidade total: " << Quantidade << " arquivo(s)\n";
	}

	void RenomearArquivo(wstring Arquivo, wstring NovoNome)
	{
		FtpRenameFile(Autenticar, Arquivo.c_str(), NovoNome.c_str());
	}

	void BaixarArquivoDoServidor(wstring ArquivoRemoto, wstring Local)
	{
		FtpGetFile(Autenticar, ArquivoRemoto.c_str(), Local.c_str(), FALSE, 0, FTP_TRANSFER_TYPE_BINARY, 0);
	}

	void FinalizarConexao()
	{
		InternetCloseHandle(Autenticar);
		InternetCloseHandle(Solicitar);
	}

}Funcoes;

int main()
{

	cout << "O assistente está executando operações de autenticação e dados no servidor FTP...\n";

	if (Funcoes.InicializarSolicitacaoFTP(L"Servidor", L"Usuario", L"12345678") == true)
	{
		cout << "Sucesso na operação de autenticação..\n";
		cout << "Executando operações de arquivos..\n";

		Funcoes.CriarDiretorio(L"Teste", true);
		Funcoes.EnviarArquivo(L"F:\\Teste.exe", L"Teste.exe");

		Funcoes.RenomearArquivo(L"Teste.exe", L"Renomeado.exe");

		Funcoes.DeletarArquivo(L"G:\\Renomeado.exe");

		cout << "Arquivos encontrados:\n";
		//Irá listar todos os arquivos referente ao diretório da conta de autenticação.
		Funcoes.ListarArquivos(L"");

		Funcoes.BaixarArquivoDoServidor(L"teste.rar", L"C:\\teste.rar");

		//Finalize a conexão entre cliente e servidor quando não for mais necessário.
		Funcoes.FinalizarConexao();
	}

	system("pause");
}
