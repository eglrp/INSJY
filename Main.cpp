#include "INSFile.h"
#include "INSUpdater.h"
int main()
{
	try {
		char * filename = "C:\\Users\\Ñó\\Desktop\\¹ßµ¼ÊµÏ°\\Data\\sbgdata2.txt";
		INSUpdater updater = INSUpdater(
			INSFile::open(filename, SBGtxt),
			ResultFile::create("result.txt")
		);
		while (updater.available())
		{
			updater.update();
		}
		updater.dispose();
	}
	catch (char * error)
	{
		printf("runtime %s\n",error);
		system("pause");
		exit(0);
	}

}