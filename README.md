# db-sync-app

---

##  API запросы:

### GET
* `/api/street_types` — возвращает таблицу address_street_type в json формате.
* `/api/place_types` — возвращает таблицу address_place_type в json формате.
* `/api/soato` — возвращает таблицу address_soato в json формате.
* `/api/street_soato/<id>` — GET /api/street_soato/\<id> -возвращает строки в json формате из таблицы address_street, soato_id которых соответствует id.

### POST
* `POST /api/add/address_soato`
    ```json
    {
        "code_soato": "string",
        "area": "string/null", 
        "district": "string/null", 
        "region": "string", 
        "place_type_id": 1,
        "place": "string"
    }
    ```

* `POST /api/add/address_street`
    ```json
    {
        "soato_id": 1,
        "street_type_id": 1,
        "name": "string"
    }
    ```

* `POST /api/add/address`
    ```json
    {
        "soato_id": 1,
        "street_id": 1,
        "house": 1,
        "corps": "string/null",
        "flat": "string/null",
        "zip_code": "string/null",
        "note": "string/null"
    }
    ```

### Синхронизация
* `GET /api/sync/all?since=TIMESTAMP` - Возвращает изменения из всех таблиц, где `last_update > since`. Формат времени: `YYYY-MM-DD HH:MM:SS`.
* `GET /api/sync/start` -  Запуск фонового процесса синхронизации с удаленным сервером (адрес сервера (remote_url) берется из `config.json`).