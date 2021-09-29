#include <iostream>
#include <string>
#include <cstring>
#include <clocale>
#include <cstdlib> // для функций rand() и srand()
#include <ctime> // для функции time()
#include <fstream>
using namespace std;

string getStr(char ch) {
    string str = "";
    str.push_back(ch);
    return str;
}

const int AIR_TICKET_ARR_LENGTH = 10;
const int MAX_STRING_LENGTH = 40;
const int AIRPORT_CODE_LENGTH = 3;
const int CURRENCY_SING_LENGTH = 3;
const int FLIGHT_NUMBER_AIRLINE_CODE_LENGTH = 2;


enum FIELD {
    FLIGHT_NUMBER_FIELD,
    DESTINATION_FIELD,
    DEPARTURE_TIME_FIELD,
    DEPARTURE_DATE_FIELD,
    PRICE_FIELD,
    IS_FULL_FIELD,
};

struct FlighNumber {
    char airline[FLIGHT_NUMBER_AIRLINE_CODE_LENGTH + 1]; // код компании: из двух цифр или букв
    int number; // номер рейса из четырёх цифр
};

struct Destination {
    char city[MAX_STRING_LENGTH + 1]; // город
    char country[MAX_STRING_LENGTH + 1]; // страна
    char airport[AIRPORT_CODE_LENGTH + 1]; // 3-х буквенный код аэропорта
};

struct DepartureTime {
    int hour;
    int minute;
};

struct DepartureDate {
    int day;
    int month;
    int year;
};

struct Price {
    float value;
    char currency[CURRENCY_SING_LENGTH + 1]; // обозначение валюты из трёх символов
};

struct AirTicket {
    struct FlighNumber flighNumber;
    struct Destination destination;
    struct DepartureTime departureTime;
    struct DepartureDate departureDate;
    struct Price price;
    bool isFull;
};

// база существующих билетов
struct DataBaseOfAirTickets {
    struct AirTicket* arr;
    int amountOfTickets; // кол-во используемых записей в БД
};

class AirTicketDataBaseController {
private:
    DataBaseOfAirTickets dataBaseOfAirTickets;

    // создаёт рандомную строку из заглавных букв английского алфавита
    void randomizeString(const int length, char* str) {
        for (int i = 0; i < length - 1; i++) {
            str[i] = 65 + rand() % 25; // символы от "A" до "Z"
        }
        str[length - 1] = '\0';
    }

    // присваивает одной строке другую
    void assignStr(char* str1, char* str2) {
        int lengthOfStr2 = 0;
        char currentChar = str2[lengthOfStr2];
        while (currentChar != '\0') {
            currentChar = str2[++lengthOfStr2];
        }
        lengthOfStr2++;

        for (int i = 0; i < lengthOfStr2; i++) {
            str1[i] = str2[i];
        }
    }

    // сравнивает номера рейсов (цифры и буквы вместе)
    // буквы и цифры сравниваются по таблице символов
    int compareFlightNumber(struct AirTicket ticket1, struct AirTicket ticket2) {
        char str1[MAX_STRING_LENGTH];
        char str2[MAX_STRING_LENGTH];

        sprintf_s(str1, "%s%d",
            ticket1.flighNumber.airline,
            ticket1.flighNumber.number);

        sprintf_s(str2, "%s%d",
            ticket2.flighNumber.airline,
            ticket2.flighNumber.number);

        return strcmp(str1, str2);
    }

    // сравнивает место назначения (страна, город, аэропорт вместе)
    // буквы сравниваются по таблице символов
    int compareDestination(struct AirTicket ticket1, struct AirTicket ticket2) {
        char str1[MAX_STRING_LENGTH];
        char str2[MAX_STRING_LENGTH];

        sprintf_s(str1, "%s%s%s",
            ticket1.destination.country,
            ticket1.destination.city,
            ticket1.destination.airport);

        sprintf_s(str2, "%s%s%s",
            ticket2.destination.country,
            ticket2.destination.city,
            ticket2.destination.airport);

        return strcmp(str1, str2);
    }

    // сравнивает время
    int compareTime(struct AirTicket ticket1, struct AirTicket ticket2) {
        return (ticket1.departureTime.hour * 60 + ticket1.departureTime.minute)
            - (ticket2.departureTime.hour * 60 + ticket2.departureTime.minute);
    }

    // сравнивает дату
    int compareDate(struct AirTicket ticket1, struct AirTicket ticket2) {
        return ((ticket1.departureDate.year * 12 + ticket1.departureDate.month) * 31 + ticket1.departureDate.day)
            - ((ticket2.departureDate.year * 12 + ticket2.departureDate.month) * 31 + ticket2.departureDate.day);
    }

    // сравнивает цену
    int comparePrice(struct AirTicket ticket1, struct AirTicket ticket2) {
        return ticket1.price.value - ticket2.price.value;
    }

    // сравнивает по любому из полей
    int compareAnyField(struct AirTicket ticket1, struct AirTicket ticket2, FIELD field) {
        if (field == IS_FULL_FIELD) {
            if (ticket1.isFull && !ticket2.isFull) {
                return 1;
            }
            if (!ticket1.isFull && ticket2.isFull) {
                return -1;
            }
            if (!ticket1.isFull && !ticket2.isFull) {
                return -1; // если оба пустые - опускаем один из них вниз списка
            }
            return 0;
        }

        if (field == FLIGHT_NUMBER_FIELD) {
            return compareFlightNumber(ticket1, ticket2);
        }
        if (field == DESTINATION_FIELD) {
            return compareDestination(ticket1, ticket2);
        }
        if (field == DEPARTURE_TIME_FIELD) {
            return compareTime(ticket1, ticket2);
        }
        if (field == DEPARTURE_DATE_FIELD) {
            return compareDate(ticket1, ticket2);
        }
        if (field == PRICE_FIELD) {
            return comparePrice(ticket1, ticket2);
        }
    }



public:

    AirTicketDataBaseController() {
        dataBaseOfAirTickets.arr = new AirTicket[AIR_TICKET_ARR_LENGTH];
        dataBaseOfAirTickets.amountOfTickets = 0;

        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++)
            dataBaseOfAirTickets.arr[i].isFull = false;
    }

    void clear(int index) {
        if (!dataBaseOfAirTickets.arr[index].isFull) {
            cout << "Запись, которую вы пытаетесь очистить, уже очищена." << endl;
            return;
        }
        dataBaseOfAirTickets.arr[index].isFull = false;
        dataBaseOfAirTickets.amountOfTickets--;
    }

    // позволяет пользователю ввести данные
    void setFlightProperties(int index) {
        if (dataBaseOfAirTickets.amountOfTickets > AIR_TICKET_ARR_LENGTH) {
            cout << "Нельзя внести в базу данных больше, чем " << AIR_TICKET_ARR_LENGTH << " записей." << endl;
            return;
        }

        char tempAirLineNumber[FLIGHT_NUMBER_AIRLINE_CODE_LENGTH + 1];
        int tempFlightNumber;
        cout << "Ввод параметров рейса:" << endl;
        cout << "Ввод номера рейса:" << endl;
        cout << "Введите номер авиакомпании (две буквы или цифры): ";
        cin >> tempAirLineNumber;
        cout << "Введите номер рейса (четыре цифры): ";
        cin >> tempFlightNumber;


        char tempCountry[MAX_STRING_LENGTH + 1];
        char tempCity[MAX_STRING_LENGTH + 1];
        char tempAirport[MAX_STRING_LENGTH + 1];
        cout << "Пункт назначения:" << endl;
        cout << "Введите страну: ";
        cin >> tempCountry;
        cout << "Введите город: ";
        cin >> tempCity;
        cout << "Введите код аэропорта (три буквы): ";
        cin >> tempAirport;

        char tempTime[6];
        string tempHoursStr;
        string tempMinutesStr;
        int tempHours;
        int tempMinutes;
        cout << "Время отправления:" << endl;
        cout << "Введите время отправления (в формате 'ЧЧ:ММ'): ";
        cin >> tempTime;
        tempHoursStr = getStr(tempTime[0]) + getStr(tempTime[1]);
        tempHours = stoi(tempHoursStr);
        tempMinutesStr = getStr(tempTime[3]) + getStr(tempTime[4]);
        tempMinutes = stoi(tempMinutesStr);

        char tempDate[11];
        string tempDayStr;
        string tempMonthStr;
        string tempYearStr;
        int tempDay;
        int tempMonth;
        int tempYear;
        cout << "Дата отправления:" << endl;
        cout << "Введите дату отправления (в формате 'ДД.ММ.ГГГГ'): ";
        cin >> tempDate;
        tempDayStr = getStr(tempDate[0]) + getStr(tempDate[1]);
        tempDay = stoi(tempDayStr);
        tempMonthStr = getStr(tempDate[3]) + getStr(tempDate[4]);
        tempMonth = stoi(tempMonthStr);
        tempYearStr = getStr(tempDate[6]) + getStr(tempDate[7]) + getStr(tempDate[8]) + getStr(tempDate[9]);
        tempYear = stoi(tempYearStr);

        char tempCurrency[CURRENCY_SING_LENGTH + 1];
        int tempValue;
        cout << "Стоимость билета:" << endl;
        cout << "Введите обозначение валюты (три буквы): ";
        cin >> tempCurrency;
        cout << "Введите значение стоимости билета (целое число): ";
        cin >> tempValue;

        assignStr(dataBaseOfAirTickets.arr[index].flighNumber.airline, tempAirLineNumber);
        dataBaseOfAirTickets.arr[index].flighNumber.number = tempFlightNumber;

        assignStr(dataBaseOfAirTickets.arr[index].destination.country, tempCountry);
        assignStr(dataBaseOfAirTickets.arr[index].destination.city, tempCity);
        assignStr(dataBaseOfAirTickets.arr[index].destination.airport, tempAirport);

        dataBaseOfAirTickets.arr[index].departureTime.hour = tempHours;
        dataBaseOfAirTickets.arr[index].departureTime.minute = tempMinutes;

        dataBaseOfAirTickets.arr[index].departureDate.day = tempDay;
        dataBaseOfAirTickets.arr[index].departureDate.month = tempMonth;
        dataBaseOfAirTickets.arr[index].departureDate.year = tempYear;

        assignStr(dataBaseOfAirTickets.arr[index].price.currency, tempCurrency);
        dataBaseOfAirTickets.arr[index].price.value = tempValue;

        dataBaseOfAirTickets.arr[index].isFull = true;

        dataBaseOfAirTickets.amountOfTickets++;
    }

    // выводит поля по индексу
    void printFlightProperties(int index) {
        if (!dataBaseOfAirTickets.arr[index].isFull) {
            cout << "Запрашиваемая запись пуста" << endl;
            return;
        }

        cout << "Параметры полёта: " << endl;

        cout << "Номер рейса: "
            << dataBaseOfAirTickets.arr[index].flighNumber.airline << " "
            << dataBaseOfAirTickets.arr[index].flighNumber.number << "\n";

        cout << "Место прибытия: "
            << dataBaseOfAirTickets.arr[index].destination.country << ", "
            << dataBaseOfAirTickets.arr[index].destination.city << ", аэропорт "
            << dataBaseOfAirTickets.arr[index].destination.airport << "\n";

        const string dayStr = dataBaseOfAirTickets.arr[index].departureDate.day < 10
            ? '0' + to_string(dataBaseOfAirTickets.arr[index].departureDate.day)
            : to_string(dataBaseOfAirTickets.arr[index].departureDate.day);

        const string monthStr = dataBaseOfAirTickets.arr[index].departureDate.month < 10
            ? '0' + to_string(dataBaseOfAirTickets.arr[index].departureDate.month)
            : to_string(dataBaseOfAirTickets.arr[index].departureDate.month);

        const string yearStr = to_string(dataBaseOfAirTickets.arr[index].departureDate.year);

        const string hourStr = dataBaseOfAirTickets.arr[index].departureTime.hour < 10
            ? '0' + to_string(dataBaseOfAirTickets.arr[index].departureTime.hour)
            : to_string(dataBaseOfAirTickets.arr[index].departureTime.hour);

        const string minuteStr = dataBaseOfAirTickets.arr[index].departureTime.minute < 10
            ? '0' + to_string(dataBaseOfAirTickets.arr[index].departureTime.minute)
            : to_string(dataBaseOfAirTickets.arr[index].departureTime.minute);


        cout << "Время отправления: "
            << dayStr << "."
            << monthStr << "."
            << yearStr << " "
            << hourStr << ":"
            << minuteStr << "\n";

        cout << "Стоимость билета: "
            << dataBaseOfAirTickets.arr[index].price.value << " "
            << dataBaseOfAirTickets.arr[index].price.currency << "\n\n";
    }

    // ищет первый пустой
    int findEmpty() {
        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++) {
            if (!dataBaseOfAirTickets.arr[i].isFull) return i;
        }

        return -1;
    }

    // выводит поля всех заполненных записей
    void printFull() {
        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++) {
            if (dataBaseOfAirTickets.arr[i].isFull) this->printFlightProperties(i);
        }
    }


    // находит и возвращает запись по заданному полю (число и строка)
    AirTicket findByField(FIELD field, int requaredValue, string requaredString) {

        AirTicket result;
        AirTicket tempStruct;
        tempStruct.isFull = true;
        switch (field)
        {
        case FLIGHT_NUMBER_FIELD:
            tempStruct.flighNumber.number = requaredValue;
            tempStruct.flighNumber.airline[0] = requaredString[0];
            tempStruct.flighNumber.airline[1] = requaredString[1];
            tempStruct.flighNumber.airline[2] = '\0';
            break;
        case PRICE_FIELD:
            tempStruct.price.value = requaredValue;
            tempStruct.price.currency[0] = requaredString[0];
            tempStruct.price.currency[1] = requaredString[1];
            tempStruct.price.currency[2] = requaredString[2];
            tempStruct.price.currency[3] = '\0';
            break;
        default:
            break;
        }

        int smallestDifference = INT_MAX;
        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++) {
            if (!dataBaseOfAirTickets.arr[i].isFull) continue;
            int currentDifference = abs(compareAnyField(tempStruct, dataBaseOfAirTickets.arr[i], field));
            if (currentDifference < smallestDifference) {
                smallestDifference = currentDifference;
                result = dataBaseOfAirTickets.arr[i];
            }
        }

        return result;
    }

    // находит и возвращает запись по заданному полю,
    // в данном случае перегрузка только для destination
    AirTicket findByField(
        FIELD field,
        string requaredCountry,
        string requaredCity,
        string requaredAirport)
    {
        AirTicket result;
        AirTicket tempStruct;
        tempStruct.isFull = true;
        switch (field)
        {
        case DESTINATION_FIELD:
            int i;
            for (i = 0; i < requaredCountry.length(); i++)
                tempStruct.destination.country[i] = requaredCountry[i];
            tempStruct.destination.country[i] = '\0';

            for (i = 0; i < requaredCity.length(); i++)
                tempStruct.destination.city[i] = requaredCity[i];
            tempStruct.destination.city[i] = '\0';

            for (i = 0; i < requaredAirport.length(); i++)
                tempStruct.destination.airport[i] = requaredAirport[i];
            tempStruct.destination.airport[i] = '\0';
            break;
        default:
            break;
        }

        int smallestDifference = INT_MAX;
        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++) {
            if (!dataBaseOfAirTickets.arr[i].isFull) continue;
            int currentDifference = abs(compareAnyField(tempStruct, dataBaseOfAirTickets.arr[i], field));
            if (currentDifference < smallestDifference) {
                smallestDifference = currentDifference;
                result = dataBaseOfAirTickets.arr[i];
            }
        }

        return result;
    }

    // находит и возвращает запись по заданному полю,
    // в данном случае перегрузка только для departureTime
    AirTicket findByField(
        FIELD field,
        int requaredHour,
        int requaredMinute)
    {
        AirTicket result;
        AirTicket tempStruct;
        tempStruct.isFull = true;
        switch (field)
        {
        case DEPARTURE_TIME_FIELD:
            tempStruct.departureTime.hour = requaredHour;
            tempStruct.departureTime.minute = requaredMinute;
        default:
            break;
        }

        int smallestDifference = INT_MAX;
        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++) {
            if (!dataBaseOfAirTickets.arr[i].isFull) continue;
            int currentDifference = abs(compareAnyField(tempStruct, dataBaseOfAirTickets.arr[i], field));
            if (currentDifference < smallestDifference) {
                smallestDifference = currentDifference;
                result = dataBaseOfAirTickets.arr[i];
            }
        }

        return result;
    }

    // находит и возвращает запись по заданному полю,
    // в данном случае перегрузка только для departureDate
    AirTicket findByField(
        FIELD field,
        int requaredDay,
        int requaredMonth,
        int requaredYear)
    {
        AirTicket result;
        AirTicket tempStruct;
        tempStruct.isFull = true;
        switch (field)
        {
        case DEPARTURE_DATE_FIELD:
            tempStruct.departureDate.day = requaredDay;
            tempStruct.departureDate.month = requaredMonth;
            tempStruct.departureDate.year = requaredYear;
        default:
            break;
        }

        int smallestDifference = INT_MAX;
        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++) {
            if (!dataBaseOfAirTickets.arr[i].isFull) continue;
            int currentDifference = abs(compareAnyField(tempStruct, dataBaseOfAirTickets.arr[i], field));
            if (currentDifference < smallestDifference) {
                smallestDifference = currentDifference;
                result = dataBaseOfAirTickets.arr[i];
            }
        }

        return result;
    }


    // находит и возвращает запись с минимальным значением заданного поля
    AirTicket findMinimalOfFiled(FIELD field) {
        int amountOfFull = 0;
        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++)
            if (dataBaseOfAirTickets.arr[i].isFull)
                amountOfFull++;

        int sortedTempArrCounter = 0;
        struct AirTicket* sortedTempArr = new struct AirTicket[amountOfFull];
        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++) {
            if (dataBaseOfAirTickets.arr[i].isFull) {
                sortedTempArr[sortedTempArrCounter] = dataBaseOfAirTickets.arr[i];
                sortedTempArrCounter++;
            }
        }


        AirTicket temp;
        for (int i = 0; i < amountOfFull; i++) {

            for (int j = 0; j < amountOfFull - i - 1; j++) {
                bool comparisonResult;
                if (compareAnyField(sortedTempArr[j], sortedTempArr[j + 1], field) >= 0) comparisonResult = true;
                else comparisonResult = false;

                if (comparisonResult) {
                    temp = sortedTempArr[j];
                    sortedTempArr[j] = sortedTempArr[j + 1];
                    sortedTempArr[j + 1] = temp;
                }
            }

        }

        temp = sortedTempArr[0];
        delete[] sortedTempArr;

        return temp;
    }

    // сортирует имеющийся массив в экземпляре класса в порядке возрастания
    void ascendingSortByField(FIELD field) {
        int amountOfFull = 0;
        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++)
            if (dataBaseOfAirTickets.arr[i].isFull)
                amountOfFull++;

        int sortedTempArrCounter = 0;
        struct AirTicket* sortedTempArr = new struct AirTicket[amountOfFull];
        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++) {
            if (dataBaseOfAirTickets.arr[i].isFull) {
                sortedTempArr[sortedTempArrCounter] = dataBaseOfAirTickets.arr[i];
                sortedTempArrCounter++;
            }
        }

        AirTicket temp;
        for (int i = 0; i < amountOfFull; i++) {

            for (int j = 0; j < amountOfFull - i - 1; j++) {
                bool comparisonResult;
                if (compareAnyField(sortedTempArr[j], sortedTempArr[j + 1], field) >= 0) comparisonResult = true;
                else comparisonResult = false;

                if (comparisonResult) {
                    temp = sortedTempArr[j];
                    sortedTempArr[j] = sortedTempArr[j + 1];
                    sortedTempArr[j + 1] = temp;
                }
            }

        }

        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++) if (dataBaseOfAirTickets.arr[i].isFull) clear(i);
        for (int i = 0; i < amountOfFull; i++) dataBaseOfAirTickets.arr[i] = sortedTempArr[i];
    }

    // сортирует имеющийся массив в экземпляре класса в порядке убывания
    void descendingSortByField(FIELD field) {
        int amountOfFull = 0;
        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++)
            if (dataBaseOfAirTickets.arr[i].isFull)
                amountOfFull++;

        int sortedTempArrCounter = 0;
        struct AirTicket* sortedTempArr = new struct AirTicket[amountOfFull];
        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++) {
            if (dataBaseOfAirTickets.arr[i].isFull) {
                sortedTempArr[sortedTempArrCounter] = dataBaseOfAirTickets.arr[i];
                sortedTempArrCounter++;
            }
        }

        AirTicket temp;
        for (int i = 0; i < amountOfFull; i++) {

            for (int j = 0; j < amountOfFull - i - 1; j++) {
                bool comparisonResult;
                if (compareAnyField(sortedTempArr[j], sortedTempArr[j + 1], field) <= 0) comparisonResult = true;
                else comparisonResult = false;

                if (comparisonResult) {
                    temp = sortedTempArr[j];
                    sortedTempArr[j] = sortedTempArr[j + 1];
                    sortedTempArr[j + 1] = temp;
                }
            }

        }

        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++) if (dataBaseOfAirTickets.arr[i].isFull) clear(i);
        for (int i = 0; i < amountOfFull; i++) dataBaseOfAirTickets.arr[i] = sortedTempArr[i];
    }

    // назначает рандомные поля одной записи по заданному индексу
    void setRandomTicket(int index) {
        if (dataBaseOfAirTickets.amountOfTickets > AIR_TICKET_ARR_LENGTH) {
            cout << "Нельзя внести в базу данных больше, чем " << AIR_TICKET_ARR_LENGTH << " записей." << endl;
            return;
        }


        randomizeString(FLIGHT_NUMBER_AIRLINE_CODE_LENGTH + 1, dataBaseOfAirTickets.arr[index].flighNumber.airline);
        dataBaseOfAirTickets.arr[index].flighNumber.number = 1000 + rand() % 3000;
        randomizeString(8, dataBaseOfAirTickets.arr[index].destination.country);
        randomizeString(8, dataBaseOfAirTickets.arr[index].destination.city);
        randomizeString(AIRPORT_CODE_LENGTH + 1, dataBaseOfAirTickets.arr[index].destination.airport);
        dataBaseOfAirTickets.arr[index].departureTime.hour = rand() % 23;
        dataBaseOfAirTickets.arr[index].departureTime.minute = rand() % 59;
        dataBaseOfAirTickets.arr[index].departureDate.day = 1 + rand() % 27;
        dataBaseOfAirTickets.arr[index].departureDate.month = 1 + rand() % 11;
        dataBaseOfAirTickets.arr[index].departureDate.year = 2000 + rand() % 30;
        randomizeString(CURRENCY_SING_LENGTH + 1, dataBaseOfAirTickets.arr[index].price.currency);
        dataBaseOfAirTickets.arr[index].price.value = 1 + rand() % 5000;;


        dataBaseOfAirTickets.arr[index].isFull = true;

        dataBaseOfAirTickets.amountOfTickets++;
    }

    // выводит только заполненные записи
    void printOnlyFull() {
        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++) {
            if (dataBaseOfAirTickets.arr[i].isFull) printFlightProperties(i);
        }
    }

    // считывает данные из файла с заданным именем и выводит их
    void saveAirTicketDataBase(char* fileName) {
        fstream fs;
        fs.open(fileName, fstream::out);
        if (!fs.is_open()) {
            cout << "Не удалось открыть файл" << endl;
            return;
        }


        for (int i = 0; i < AIR_TICKET_ARR_LENGTH; i++) {
            if (dataBaseOfAirTickets.arr[i].isFull) {
                const string dayStr = dataBaseOfAirTickets.arr[i].departureDate.day < 10
                    ? '0' + to_string(dataBaseOfAirTickets.arr[i].departureDate.day)
                    : to_string(dataBaseOfAirTickets.arr[i].departureDate.day);

                const string monthStr = dataBaseOfAirTickets.arr[i].departureDate.month < 10
                    ? '0' + to_string(dataBaseOfAirTickets.arr[i].departureDate.month)
                    : to_string(dataBaseOfAirTickets.arr[i].departureDate.month);

                const string yearStr = to_string(dataBaseOfAirTickets.arr[i].departureDate.year);

                const string hourStr = dataBaseOfAirTickets.arr[i].departureTime.hour < 10
                    ? '0' + to_string(dataBaseOfAirTickets.arr[i].departureTime.hour)
                    : to_string(dataBaseOfAirTickets.arr[i].departureTime.hour);

                const string minuteStr = dataBaseOfAirTickets.arr[i].departureTime.minute < 10
                    ? '0' + to_string(dataBaseOfAirTickets.arr[i].departureTime.minute)
                    : to_string(dataBaseOfAirTickets.arr[i].departureTime.minute);

                fs
                    << dataBaseOfAirTickets.arr[i].flighNumber.airline
                    << dataBaseOfAirTickets.arr[i].flighNumber.number
                    << ","
                    << dataBaseOfAirTickets.arr[i].destination.country << " "
                    << dataBaseOfAirTickets.arr[i].destination.city << " "
                    << dataBaseOfAirTickets.arr[i].destination.airport
                    << ","
                    << dayStr << "."
                    << monthStr << "."
                    << yearStr
                    << ","
                    << hourStr << ":"
                    << minuteStr
                    << ","
                    << dataBaseOfAirTickets.arr[i].price.currency
                    << dataBaseOfAirTickets.arr[i].price.value
                    << ";";
            }
        }

        fs.close();

    }

    // считывает данные из файла с заданным именем и выводит их
    void readAirTicketDataBase(char* fileName) {
        fstream fs;
        fs.open(fileName, fstream::in);
        if (!fs.is_open()) {
            cout << "Не удалось открыть файл" << endl;
            return;
        }

        while (!fs.eof()) {
            string message = "";
            fs >> message;
            cout << message;
        }


        fs.close();
    }

};

int main() {
    setlocale(LC_ALL, "rus");
    srand(static_cast<unsigned int>(time(0)));

    AirTicketDataBaseController db;
    db.setRandomTicket(3);
    db.setRandomTicket(6);
    db.setRandomTicket(7);
    db.setFlightProperties(8);

    db.printFlightProperties(3);
    db.printFlightProperties(6);
    db.printFlightProperties(7);
    db.printFlightProperties(8);

    db.printOnlyFull();

    cout << db.findByField(DESTINATION_FIELD, "AAA", "AAA", "AAA").flighNumber.number << endl;

    return 0;
}
