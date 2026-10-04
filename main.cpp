#include <iostream>
#include <memory>
#include <string>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>
#include "models.h"

namespace dbo = Wt::Dbo;

int main() {
    try {
        // ---------- 1. Подключение к PostgreSQL ----------
        auto postgres = std::make_unique<dbo::backend::Postgres>(
            "host=localhost port=5432 dbname=postgres user=postgres password=zoom1989"
        );

        dbo::Session session;
        session.setConnection(std::move(postgres));

        // ---------- 2. Регистрация ORM-классов ----------
        session.mapClass<Publisher>("publisher");
        session.mapClass<Book>("book");
        session.mapClass<Shop>("shop");
        session.mapClass<Stock>("stock");
        session.mapClass<Sale>("sale");

        // ---------- 3. Создание таблиц ----------
        session.createTables();
        std::cout << "Таблицы успешно созданы (или уже существовали)." << std::endl;

        // ---------- 4. Заполнение тестовыми данными ----------
        {
            dbo::Transaction t(session);

            auto count = session.query<int>("SELECT COUNT(*) FROM publisher").resultValue();
            if (count == 0) {
                // Издатели
                auto pub1 = session.add(std::make_unique<Publisher>());
                pub1.modify()->name = "Питер";
                auto pub2 = session.add(std::make_unique<Publisher>());
                pub2.modify()->name = "Эксмо";

                // Магазины
                auto shop1 = session.add(std::make_unique<Shop>());
                shop1.modify()->name = "Буквоед";
                auto shop2 = session.add(std::make_unique<Shop>());
                shop2.modify()->name = "Читай-город";
                auto shop3 = session.add(std::make_unique<Shop>());
                shop3.modify()->name = "Лабиринт";

                // Книги
                auto b1 = session.add(std::make_unique<Book>());
                b1.modify()->title = "C++ для профи";
                b1.modify()->publisher = pub1;

                auto b2 = session.add(std::make_unique<Book>());
                b2.modify()->title = "Паттерны проектирования";
                b2.modify()->publisher = pub1;

                auto b3 = session.add(std::make_unique<Book>());
                b3.modify()->title = "Чистый код";
                b3.modify()->publisher = pub2;

                // Склад
                auto s1 = session.add(std::make_unique<Stock>());
                s1.modify()->book = b1;
                s1.modify()->shop = shop1;
                s1.modify()->count = 10;

                auto s2 = session.add(std::make_unique<Stock>());
                s2.modify()->book = b1;
                s2.modify()->shop = shop2;
                s2.modify()->count = 5;

                auto s3 = session.add(std::make_unique<Stock>());
                s3.modify()->book = b2;
                s3.modify()->shop = shop3;
                s3.modify()->count = 7;

                auto s4 = session.add(std::make_unique<Stock>());
                s4.modify()->book = b3;
                s4.modify()->shop = shop1;
                s4.modify()->count = 3;

                // Продажи
                auto sale1 = session.add(std::make_unique<Sale>());
                sale1.modify()->price = 1500.0;
                sale1.modify()->date_sale = "2024-01-15";
                sale1.modify()->stock = s1;
                sale1.modify()->count = 2;

                t.commit();
                std::cout << "Тестовые данные добавлены." << std::endl;
            }
            else {
                std::cout << "Данные уже есть в базе." << std::endl;
            }
        }

        // ---------- 5. Запрос у пользователя ----------
        std::string input;
        std::cout << "Введите имя или ID издателя: ";
        std::getline(std::cin, input);

        // ---------- 6. Поиск магазинов ----------
        {
            dbo::Transaction t(session);
            dbo::collection<dbo::ptr<Publisher>> publishers;
            try {
                int id = std::stoi(input);
                publishers = session.find<Publisher>().where("id = ?").bind(id);
            }
            catch (...) {
                publishers = session.find<Publisher>().where("name = ?").bind(input);
            }

            if (publishers.empty()) {
                std::cout << "Издатель не найден." << std::endl;
                return 0;
            }

            auto publisher = *publishers.begin();
            std::cout << "\nИздатель: " << publisher->name
                << " (ID=" << publisher->id << ")" << std::endl;
            std::cout << "Магазины, где продаются его книги:" << std::endl;

            auto shops = session.query<dbo::ptr<Shop>>(
                "SELECT DISTINCT s FROM shop s "
                "JOIN stock st ON st.id_shop = s.id "
                "JOIN book b ON st.id_book = b.id "
                "WHERE b.id_publisher = ?"
            ).bind(publisher.id());

            int i = 1;
            for (const auto& shop : shops.resultList()) {
                std::cout << i++ << ". " << shop->name << std::endl;
            }

            if (i == 1) {
                std::cout << "Магазинов не найдено." << std::endl;
            }
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}