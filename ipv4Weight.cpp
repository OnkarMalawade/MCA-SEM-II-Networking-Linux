#include <iostream>
#include <bits/stdc++.h>
using namespace std;

// pahilia address mojnya sathi 
void calculateNetworkAddress(int networkAddress[4], int ipArray[4], int subnetMask) {
    for (int i = 0; i < 4; i++) {
        if (subnetMask >= 8) { // ek tun cut karaych 
            networkAddress[i] = ipArray[i];
            subnetMask -= 8;
        } else { 
            networkAddress[i] = (ipArray[i] >> (8 - subnetMask)) << (8 - subnetMask);
            break;
        }
    }
}

// dusra address mojnya sathi
void calculateBroadcastAddress(int broadcastAddress[4], int ipArray[4], int subnetMask) {
    for (int i = 0; i < 4; i++) {
        if (subnetMask >= 8) {
            broadcastAddress[i] = ipArray[i];
            subnetMask -= 8;
        } else { // magun krych
            broadcastAddress[i] = (ipArray[i] >> (8 - subnetMask)) << (8 - subnetMask);
            // | use kelne bitwise or operator ahe extra 1111 yetil
			broadcastAddress[i] |= (1 << (8 - subnetMask)) - 1; // Set host bits to 1
            break;
        }
    }
}

int totalAddress(int n){
	// pow bits/c++ waprlay
	int p = 32 - n;
	int k = pow(2,p);
	return k;
}

int main() {
	int n = 4;
    int ipAddress[n];
    cout << "\n Enter Ip Address: " << endl;
    for(int i = 0; i < n; i++){
    	cin >> ipAddress[i];
	}
    int subnetMask;
    cout << "\n Subnet Weight mask: ";
    cin >> subnetMask;

    int networkAddress[n];
    calculateNetworkAddress(networkAddress, ipAddress, subnetMask);

    int broadcastAddress[n];
    calculateBroadcastAddress(broadcastAddress, ipAddress, subnetMask);

    cout << "Given IPv4 address: " << ipAddress[0] << "." << ipAddress[1] << "." << ipAddress[2] << "." << ipAddress[3] << "/" << subnetMask << endl;
	// pahila wala
    cout << "First Address: " << networkAddress[0] << "." << networkAddress[1] << "." << networkAddress[2] << "." << networkAddress[3] << endl;
    // dusra wala
    cout << "Last Address: " << broadcastAddress[0] << "." << broadcastAddress[1] << "." << broadcastAddress[2] << "." << broadcastAddress[3] << endl;
	// kiti address astil 
	cout << "Total Address: " << totalAddress(subnetMask);
    return 0;
}
