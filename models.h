#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/WDate.h>

namespace dbo = Wt::Dbo;

class Publisher;
class Book;
class Shop;
class Stock;
class Sale;

// ---------- Publisher ----------
class Publisher {
public:
    int id = 0;
    std::string name;

    template<class Action>
    void persist(Action& a) {
        dbo::id(a, id, "id");
        dbo::field(a, name, "name");
        dbo::hasMany(a, books, dbo::ManyToOne, "id_publisher");
    }

private:
    dbo::collection<dbo::ptr<Book>> books;
};

// ---------- Book ----------
class Book {
public:
    int id = 0;
    std::string title;
    dbo::ptr<Publisher> publisher;

    template<class Action>
    void persist(Action& a) {
        dbo::id(a, id, "id");
        dbo::field(a, title, "title");
        dbo::belongsTo(a, publisher, "id_publisher");
        dbo::hasMany(a, stocks, dbo::ManyToOne, "id_book");
    }

private:
    dbo::collection<dbo::ptr<Stock>> stocks;
};

// ---------- Shop ----------
class Shop {
public:
    int id = 0;
    std::string name;

    template<class Action>
    void persist(Action& a) {
        dbo::id(a, id, "id");
        dbo::field(a, name, "name");
        dbo::hasMany(a, stocks, dbo::ManyToOne, "id_shop");
    }

private:
    dbo::collection<dbo::ptr<Stock>> stocks;
};

// ---------- Stock ----------
class Stock {
public:
    int id = 0;
    dbo::ptr<Book> book;
    dbo::ptr<Shop> shop;
    int count = 0;

    template<class Action>
    void persist(Action& a) {
        dbo::id(a, id, "id");
        dbo::belongsTo(a, book, "id_book");
        dbo::belongsTo(a, shop, "id_shop");
        dbo::field(a, count, "count");
        dbo::hasMany(a, sales, dbo::ManyToOne, "id_stock");
    }

private:
    dbo::collection<dbo::ptr<Sale>> sales;
};

// ---------- Sale ----------
class Sale {
public:
    int id = 0;
    double price = 0.0;
    Wt::WDate date_sale;
    dbo::ptr<Stock> stock;
    int count = 0;

    template<class Action>
    void persist(Action& a) {
        dbo::id(a, id, "id");
        dbo::field(a, price, "price");
        dbo::field(a, date_sale, "date_sale");
        dbo::belongsTo(a, stock, "id_stock");
        dbo::field(a, count, "count");
    }
};

#endif // MODELS_H