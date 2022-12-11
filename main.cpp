#include <iostream>
#include "crypt.h"

using namespace std;


class Client {
private:
    lli R, N, S, V;
    string name;
    bool cheater = false;

    void generateS() {
        S = genMutPrime(N - 1);
        V = modPow(S, 2, N);
    }

public:
    lli generateR() {
        R = random(1, N - 1);
        return R;
    }

    explicit Client(lli N, const string &name = "empty") {
        this->N = N;
        generateS();
        this->name = name;
    }

    lli getV() {
        return V;
    }

    string getName() {
        return name;
    }

    lli getX() {
        generateR();
        if (cheater) {
            return cheatX();
        }
        return modPow(R, 2, N);
    }

    lli setV(lli V) {
        this->V = V;
        cheater = true;
    }

    lli getY(int E) {
        return R * modPow(S, E, N);
    }

    lli cheatX() {
        lli X;
        switch (rand() % 2) {
            case 1:
                X = R * R / V;
                break;
            default:
                X = R * R;
                break;
        }
        return X;
    }


};

class Server {
private:
    unsigned int numOfClients = 0, numOfChecks = 1;

    lli P{}, Q{}, N{};
    struct login {
        lli V;
        string name;
    };
    vector<login> clientBase;
public:
    explicit Server(int numOfChecks,lli min = INT32_MAX >> 3, lli max = INT32_MAX>>2):numOfChecks(numOfChecks) {
        P = genPrime(min, max);
        Q = genPrime(min, max);
        N = P * Q;
    }

    void signIn(lli V, string name) {
        login curr;
        for (auto &i: clientBase) {
            if (i.name == name) {
                return;
            }
        }
        curr.V = V;
        curr.name = name;
        clientBase.push_back(curr);
    }

    lli getLoginV(const string &name) {
        for (auto &i: clientBase) {
            if (i.name == name) {
                return i.V;
            }
        }
        return 0;
    }

    lli printBase() {
        for (auto &i: clientBase) {

            cout << i.name << " : " << i.V << endl;
        }
        return 0;
    }

    lli getN() {
        return N;
    }

    bool verify(Client *intruder) {
        bool verified = false;
        for (int i = 0; i < numOfChecks; i++) {
            lli X = intruder->getX();
            int E = (int) rand() % 2;
            lli Y = intruder->getY(E);
            lli c = ((Y * Y)== X * modPow(getLoginV(intruder->getName()), E, N));
//              cout<<(Y * Y)<<" "<< X * modPow(getLoginV(intruder->getName()), E, N)
//              <<endl;
            if (Y) {
                if (c) {
                    verified = true;
                } else {
                    return false;
                }
            } else {
                return false;

            }
        }
        return verified;
    }

};

int main() {
    srand(time(NULL));
    Server server = Server(7);
    lli N = server.getN();
    Client alice = Client(N, "alice");
    Client bob = Client(N, "bob");
    Client cheaterClient = Client(N, "alice");
    server.signIn(bob.getV(), bob.getName());
    server.signIn(alice.getV(), alice.getName());
    cout << "TRUE PERSON:" << server.verify(&alice) << endl;
    cout << "TRUE PERSON:" << server.verify(&bob) << endl;
    server.printBase();
    cheaterClient.setV(server.getLoginV("alice"));
    int count=0;
    while(!server.verify(&cheaterClient)){
        count++;
        if(count>1000000
        ){
            cout<<"\nLIMIT\n";break;
        }
    }
    cout << "PERSON WITH CHEATED V KEY :" <<count << endl;
    return 0;
}
