module GlobalTable{
    interface DistributedMap{
	void put(string s, string delta);
	void set(string s, string counts);
	bool remove(string s, out string counts);
	["amd"] void putNget(string s, string delta, out string counts);
	bool get(string s, out string counts);
	["amd"] void waitForAllClients();
    };
};
