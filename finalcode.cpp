#include <iostream>
#include <ctime>
#include <iomanip>
#include <fstream>
using namespace std;

const int max_Transactions = 100;
const int max_Accounts = 3;

class Person
{
protected:
    string name;

public:
    Person(string n)
    {
        this->name = n;
    }
    string getName()
    {
        return name;
    }
};

class User : public Person
{
    int pin;

public:
    User(string n = "", int p = 0) : Person(n)
    {
        pin = p;
    }
    int getPin()
    {
        return pin;
    }
    void setPin(int p)
    {
        pin = p;
    }
};

class Transaction
{
    string type, time;
    double amount;

public:
    Transaction(string t = "", double a = 0.0, string tm = "")
    {
        type = t;
        amount = a;
        time = tm;
    }

    void display()
    {
        cout << left << setw(15) << type << setw(15) << amount << setw(25) << time << endl;
    }

    string toString()
    {
        return type + "," + to_string(amount) + "," + time + "\n";
    }

    static Transaction fromString(string line)
    {
        string type, amountStr, time;
        int first = line.find(',');
        int second = line.find(',', first + 1);
        if (first == string::npos || second == string::npos)
            return Transaction();

        type = line.substr(0, first);
        amountStr = line.substr(first + 1, second - first - 1);
        time = line.substr(second + 1);
        double amount = stod(amountStr);

        return Transaction(type, amount, time);
    }
};

class BankOperations
{
public:
    virtual void deposit(double) = 0;
    virtual void withdraw(double) = 0;
    virtual void checkBalance() = 0;
};

class StatementGenerator
{
public:
    virtual void miniStatement() = 0;
    virtual void fullHistory() = 0;
};

class Account : public BankOperations, public StatementGenerator
{
    string accountNo;
    double balance;
    User user;
    Transaction history[max_Transactions];
    int count;

    string currentTime()
    {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        char timeString[30];
        strftime(timeString, 30, "%d-%m-%Y %I:%M:%S %p", ltm);
        return string(timeString);
    }

    string getFileName()
    {
        return accountNo + "_data.txt";
    }

public:
    Account(string acc = "", string name = "", int pin = 0) : user(name, pin)
    {
        accountNo = acc;
        balance = 0.0;
        count = 0;
        load();
    }

    void save()
    {
        ofstream out(getFileName());
        out << accountNo << endl
            << user.getName() << endl
            << user.getPin() << endl
            << balance << endl;
        for (int i = 0; i < count; i++)
            out << history[i].toString();
    }

    void load()
    {
        ifstream in(getFileName());
        if (!in)
            return;

        string name, line;
        int pin;
        in >> accountNo;
        in.ignore();
        getline(in, name);
        in >> pin >> balance;
        user = User(name, pin);

        in.ignore();
        count = 0;
        while (getline(in, line) && count < max_Transactions)
            history[count++] = Transaction::fromString(line);
    }

    int getPin()
    {
        return user.getPin();
    }

    bool login(string acc, int p)
    {
        return acc == accountNo && p == user.getPin();
    }

    void deposit(double amt)
    {
        if (amt <= 0)
        {
            cout << "Invalid amount!" << endl;
            return;
        }
        balance += amt;
        if (count < max_Transactions)
            history[count++] = Transaction("Deposit", amt, currentTime());
        save();
        cout << "Deposited Rs " << amt << endl;
    }

    void withdraw(double amt)
    {
        if (amt > balance)
        {
            cout << "Insufficient balance!" << endl;
            return;
        }
        balance -= amt;
        if (count < max_Transactions)
            history[count++] = Transaction("Withdraw", amt, currentTime());
        save();
        cout << "Withdrawn Rs " << amt << endl;
    }

    void checkBalance()
    {
        cout << "Balance: Rs " << balance << endl;
    }

    void miniStatement()
    {
        cout << endl
             << "--- Mini Statement ---" << endl;
        cout << "Name       : " << user.getName() << endl;
        cout << "Account No : " << accountNo << endl;
        cout << "Balance    : Rs " << balance << endl;
        cout << endl
             << "Last 3 Transactions:" << endl;
        cout << left << setw(15) << "Type" << setw(15) << "Amount" << setw(25) << "Time" << endl;
        cout << "---------------------------------------------------------" << endl;

        int start = count > 3 ? count - 3 : 0;
        for (int i = start; i < count; i++)
        {
            history[i].display();
        }
    }

    void fullHistory()
    {
        cout << "--- Transaction History ---" << endl;
        cout << left << setw(15) << "Type" << setw(15) << "Amount" << setw(25) << "Time" << endl;
        cout << "---------------------------------------------------------" << endl;
        for (int i = 0; i < count; i++)
            history[i].display();
    }

    void changePin()
    {
        int oldPin, newPin;
        cout << "Current PIN: ";
        cin >> oldPin;
        if (oldPin != user.getPin())
        {
            cout << "Incorrect PIN!" << endl;
            return;
        }
        cout << "New PIN: ";
        cin >> newPin;
        user.setPin(newPin);
        save();
        cout << "PIN changed successfully!" << endl;
    }
};

class ATM
{
    Account accounts[max_Accounts];

public:
    ATM()
    {
        accounts[0] = Account("12345678910", "M Sohaib", 1111);
        accounts[1] = Account("12345678911", "Ahsan Manzoor", 2222);
        accounts[2] = Account("12345678912", "Hasnain Ali", 3333);
    }

    Account *authenticate(string acc, int pin)
    {
        for (int i = 0; i < max_Accounts; i++)
        {
            if (accounts[i].login(acc, pin))
                return &accounts[i];
        }
        return nullptr;
    }

    void start()
    {
        while (true)
        {
            cout << endl
                 << "=== FinServe360 ===" << endl
                 << "1. Login " << endl
                 << "2. Exit " << endl
                 << "Choose: ";
            int opt;
            cin >> opt;

            if (opt == 1)
            {
                string acc;
                int pin;
                cout << "Account Number: ";
                cin >> acc;
                cout << "PIN: ";
                cin >> pin;

                Account *user = authenticate(acc, pin);
                if (!user)
                {
                    cout << "Invalid login!" << endl;
                    continue;
                }

                int ch;
                do
                {
                    cout << endl
                         << "--- Menu ---" << endl
                         << "1. Balance" << endl
                         << "2. Deposit" << endl
                         << "3. Withdraw" << endl
                         << "4. Mini Statement" << endl
                         << "5. Transaction History" << endl
                         << "7. Logout" << endl
                         << "Choose: ";
                    cin >> ch;

                    switch (ch)
                    {
                    case 1:
                        user->checkBalance();
                        break;
                    case 2:
                    {
                        double amt;
                        cout << "Amount: ";
                        cin >> amt;
                        user->deposit(amt);
                        break;
                    }
                    case 3:
                    {
                        double amt;
                        cout << "Amount: ";
                        cin >> amt;
                        user->withdraw(amt);
                        break;
                    }
                    case 4:
                        user->miniStatement();
                        break;
                    case 5:
                        user->fullHistory();
                        break;
                    case 6:
                        user->changePin();
                        break;
                    case 7:
                        cout << "Logged out!" << endl;
                        break;
                    default:
                        cout << "Invalid choice!" << endl;
                    }
                } while (ch != 7);
            }
            else if (opt == 2)
            {
                cout << "Thank you for using FinServe360!" << endl;
                break;
            }
            else
            {
                cout << "Invalid option!" << endl;
            }
        }
    }
};

int main()
{
    ATM machine;
    machine.start();
    return 0;
}