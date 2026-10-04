#include <iostream>
#include <memory>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>
#include "models.h"

namespace dbo = Wt::Dbo;

int main() {
    try {
        // Подключение к PostgreSQL (пароль замени на свой!)
        auto postgres = std::make_unique<dbo::backend::Postgres>(
            "host=localhost port=5432 dbname=postgres user=postgres password=zoom1989"
        );

        dbo::Session session;
        session.setConnection(std::move(postgres));

        // Регистрируем ORM-классы
        session.mapClass<Publisher>("publisher");
        session.mapClass<Book>("book");
        session.mapClass<Shop>("shop");
        session.mapClass<Stock>("stock");
        session.mapClass<Sale>("sale");

        // Создаём таблицы в БД
        session.createTables();
        std::cout << "Таблицы успешно созданы!" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}