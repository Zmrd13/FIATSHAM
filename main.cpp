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
    explicit Server(lli min = INT32_MAX >> 1, lli max = INT32_MAX) {
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
        cout << intruder->getName() << "  " << getLoginV(intruder->getName()) << endl;
        for (int i = 0; i < 40; i++) {
            lli X = intruder->getX();
            int E = (int) rand() % 2;
            //  cout<<E<<endl;
            lli Y = intruder->getY(E);
            lli c = ((Y * Y) == X * modPow(getLoginV(intruder->getName()), E, N));
            //  cout<<(Y * Y)<<" "<< X * modPow(getLoginV(intruder->getName()), E, N)
            //  <<endl;
            if (Y) {
                if (c) {
                    verified = true;
                } else {
                    return false;
                    // cout<<verified;
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
    Server server = Server();
    Server cheaterServer = Server();
    lli N = server.getN();
    Client alice = Client(N, "alice");
    Client bob = Client(N, "bob");
    Client cheaterClient = Client(N, "alice");
    server.signIn(bob.getV(), bob.getName());
    server.signIn(cheaterClient.getV(), cheaterClient.getName());
    server.signIn(alice.getV(), alice.getName());
    cheaterClient.setV(server.getLoginV("alice"));
    server.printBase();
    cout << "TRUE PERSON:" << server.verify(&alice) << endl;
    cout << "TRUE PERSON:" << server.verify(&bob) << endl;
    cout << "PERSON WITH CHEATED V KEY :" << server.verify(&cheaterClient) << endl;
    return 0;
}
