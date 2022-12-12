#include <iostream>
#include <iomanip>
#include "crypt.h"

using namespace std;

/**
 * Client class with name and secret
 *
 *
 * */
class Client {
private:
    lli R{}, N, S{}, V{};
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

    [[nodiscard]] lli getV() const {
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


    [[nodiscard]]lli getY(int E) const{
        return R * modPow(S, E, N);
    }

    /**
 * Func for cheating mode
 * sets own shared key as someone else's shared key
 * turns cheat mode on
 *
 * */
    void setV(lli sharedKey) {
        this->V = sharedKey;
        cheater = true;
    }

/**
 * Func for cheating mode
 * randomly chooses what to give to getX without knowing the secret but with a shared key
 * @see setV()
 *
 * */
    [[nodiscard]] lli cheatX() const {
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

/**
 * Server class with login and common keys , provides accreditation with |numOfChecks|-times
 *
 *
 * */
class Server {
private:
    unsigned int numOfChecks = 1;

    lli P{}, Q{}, N{};
    struct login {
        lli V{};
        string name;
    };
    vector<login> clientBase;
public:
    /**
     * @see Server
     * @param numOfChecks - deepness of verification
     * @param min - min for random
     * @param max - max for random
     * */
    explicit Server(int numOfChecks, lli min = INT32_MAX >> 4, lli max = INT32_MAX >> 1) : numOfChecks(numOfChecks) {
        P = genPrime(min, max);
        Q = genPrime(min, max);
        N = P * Q;
    }

    /**
     *  Func for signing clients in server's base
      * @see Server
      * @see Client
      * @param V-Client's common key
      * @param name-Client's name (unique)
      * */
    void signIn(lli V, const string& name) {
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

    /**
      *  Func for getting common key of given name
       * @see Server
       * @see Client
       * @param name-Client's name
       * */
    lli getLoginV(const string &name) {
        for (auto &i: clientBase) {
            if (i.name == name) {
                return i.V;
            }
        }
        return 0;
    }

    /**
      *  Func for printing entire client base
       * @see Server
       * */
    lli printBase() {
        for (auto &i: clientBase) {

            cout << "NAME: |" << setw(15) << i.name << "|  SHARED KEY : |" << setw(20) << i.V << "|\n";
        }
        return 0;
    }

    [[nodiscard]] lli getN() const {
        return N;
    }

    /**
     *  Func for verifying given client
      * @see Server
      * @see Client
      * @param intruder-pointer to given client
      * @return true if verified
      * */
    bool verify(Client *intruder) {
        bool verified = false;
        for (int i = 0; i < numOfChecks; i++) {
            lli X = intruder->getX();
            int E = (int) rand() % 2;
            lli Y = intruder->getY(E);
            int c = ((Y * Y) == X * modPow(getLoginV(intruder->getName()), E, N));
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
    //Initialize all components
    srand(time(nullptr));
    int times = -1;
    cout << "Number of accreditation times:\n";
    while (times < 0)
        cin >> times;
    Server server = Server(times);
    lli N = server.getN();
    Client alice = Client(N, "alice");
    Client bob = Client(N, "bob");
    Client cheaterClient = Client(N, "alice");

    //Signing in server's base
    server.signIn(bob.getV(), bob.getName());
    server.signIn(alice.getV(), alice.getName());
    string result = (server.verify(&alice)) ? "SUCCESS" : "FAIL";
    cout << "TRUE PERSON:" << result << endl;
    result = (server.verify(&bob)) ? "SUCCESS" : "FAIL";
    cout << "TRUE PERSON:" << result << endl;
    server.printBase();

    //Cheating attempt
    cheaterClient.setV(server.getLoginV("alice"));
    int count = 0;
    while (!server.verify(&cheaterClient)) {
        count++;
        if (count > 100000) {
            cout << "\nLIMIT\n";
            break;
        }
    }
    result = (count < 100000) ? "SUCCESS - " + to_string(count) : "FAIL";
    cout << "PERSON WITH CHEATED V KEY LOGIN ATTEMPTS :" << result << endl;
    return 0;
}
