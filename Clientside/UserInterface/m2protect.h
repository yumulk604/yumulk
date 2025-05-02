#include <thread>
#pragma comment( lib, "m2protect.lib" )

using namespace std;

class M2Protect
{
	public:
		bool Guard();
		void Setup(string licenceCode, DWORD mainThreadID, bool usePulicIP);
} AntiCheat;

class EncodeThis : public string
{
	public:
		EncodeThis(string str)
		{
			string phrase(str.c_str(), str.length());
			this->assign(phrase);
		}
		EncodeThis c(char c) 
		{
			string phrase(this->c_str(), this->length());
			phrase += c;
			this->assign(phrase);

			return *this;
		}
} str("");

class workingThread 
{
	public:
		void M2Protect(DWORD mainThreadID)
		{
			string myLicence = str.c('2').c('0').c('.').c('5').c('.').c('1').c('6');
			AntiCheat.Setup(myLicence, mainThreadID, true); // true is for showing hackers's IP instead of GUID (change to false if you want hackers's GUID)
			
			while (!AntiCheat.Guard())
				this_thread::sleep_for(chrono::seconds(1));
			
			PostThreadMessage(mainThreadID, WM_QUIT, 0, 0);
		}
} wT;
