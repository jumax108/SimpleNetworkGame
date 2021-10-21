#pragma once

struct stUser;

class CSingleSector {

public:

	void addUser(int id);
	void eraseUser(int id);

	__declspec(noinline) std::unordered_set<int>::iterator userListBegin() { return userList.begin(); }
	__declspec(noinline) std::unordered_set<int>::iterator userListEnd() { return userList.end(); }

private:
	std::unordered_set<int> userList;

};

class CSectorList {

public:

	CSectorList(int mapWidth, int mapHeight, int sectorWidth, int sectorHeight);
	~CSectorList();

	void addUser(int y, int x, int id);
	void eraseUser(int y, int x, int id);

	void coordinateToIndex(int coordinateY, int coordinateX, int* indexY, int* indexX);

	std::vector<CSingleSector*>* nearSector(int y, int x);

	static void eraseDuplicatedSector(std::vector<CSingleSector*>*, std::vector<CSingleSector*>*);

	CSingleSector* getSector(int y, int x) {
		return &_sector[y][x];
	}

private:

	// ���� ũ��
	int _sectorHeight;
	int _sectorWidth;

	// �� ũ��
	int _mapHeight;
	int _mapWidth;
	
	// ���� ��
	int _widthNum;
	int _heightNum;

	CSingleSector** _sector;

};