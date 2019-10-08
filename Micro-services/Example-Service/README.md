# Example Service

Пример того, как должен быть оформлен микро-сервис.

## root

Помимо поддиректорий (см. ниже) здесь хранятся следующие файлы:

* [.dockerignore](.dockerignore) - может быть полезен при повторной сборке контейнера, т.к. без этого файла он будет запинаться об какой-то файл из уже смонитрованной БД в дирректории *database/data/*;
* [.gitignore](.gitignore)
* [CMakeLists.txt](CMakeLists.txt) - основной файл для сборки с помощью cmake;
* [docker-compose.yml](docker-compose.yml) - конфиг для docker-compose, позволяющей запускать несколько контейнеров одновременно (см. [еще один пример использования](https://docs.docker.com/compose/gettingstarted/));
* [Dockerfile](Dockerfile) - описывает контейнер именно для микро-сервиса. Контейнер для БД создается непосредственно в [docker-compose.yml](docker-compose.yml);
* [README.md](README.md) - необязательный файл с пояснениями. Но было бы неплохо и красиво, если бы он был.

## src

Здесь хранится весь исходный код микро-сервиса, а так же *CMakeLists.txt* (со всеми его зависимостями), который включается в *CMakeLists.txt* корневой директории микро-сервиса (см. [здесь](https://stackoverflow.com/questions/8304190/cmake-with-include-and-source-paths-basic-setup)).

Если в проекте используются сторонние библиотеки (которых нет в [libs](../libs)), то их нужно вынести в отдельную папку *libs* этого микро-сервиса.

## database

Здесь хранится(-ятся) только файл(-ы) для первоначального построения БД при запуске *docker-compose*.

Следует учесть, что скрипт будет запускаться при каждом запуске *docker-compose*, так что стоит всегда использовать **IF NOT EXIST** в SQL-скрипте.

В поддиректорию *data/mysql* этой директории стоит монтировать БД конейнера (см. [volumes](docker-compose.yml))