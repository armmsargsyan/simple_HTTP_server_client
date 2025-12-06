Проект: простой CHLP сервер и клиент на C.
Сервер server слушает порт 4363 и работает поверх TCP. Для каждого входящего соединения создаётся поток через pthread_create, который разбирает запрос и вызывает handle_get, handle_post или handle_echo.
Клиент client — консольное приложение. Сначала спрашивает метод (GET/POST/ECHO), потом путь, потом (если нужно) тело запроса. Формат пакета: METHOD /path CHLP/1.0; строка Body-Size: N; пустая строка; дальше текст тела. Ответ сервера сразу печатается в stdout.
Файлы сервер читает и пишет только из каталога local. Создайте там текстовые файлы заранее, иначе GET вернёт 404 Not Found. GET возвращает содержимое файла, POST дописывает тело запроса в файл и отвечает 200 OK, ECHO повторяет присланное тело с тем же размером. При синтаксической ошибке сервер отвечает 400 Bad Request, при отсутствии файла 404, при ошибке записи 500.
Сборка: gcc server.c parser.c -lpthread -o server и gcc client.c -o client. Затем запускаете ./server, а в другом терминале ./client.
Пример работы клиента:
./client
Enter METHOD (GET / POST / ECHO)
ECHO
Enter PATH (example: /index.txt)
/test.txt
Enter BODY text
hello world
server responce
CHLP/1.0 200 OK
Body-Size: 11
hello world
