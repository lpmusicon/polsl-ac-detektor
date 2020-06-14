import { Injectable } from '@angular/core';
import { HttpRequest, HttpResponse, HttpHandler, HttpEvent, HttpInterceptor, HTTP_INTERCEPTORS } from '@angular/common/http';
import { Observable, of, throwError } from 'rxjs';
import { delay, mergeMap, materialize, dematerialize } from 'rxjs/operators';

// array in local storage for registered users
let users = JSON.parse(localStorage.getItem('users')) || [];

let currentWifi;
let knownWifi = [{ ssid: 'test', password: 'testtest' }];

@Injectable()
export class FakeBackendInterceptor implements HttpInterceptor {
    intercept(request: HttpRequest<any>, next: HttpHandler): Observable<HttpEvent<any>> {
        const { url, method, headers, body } = request;

        // wrap in delayed observable to simulate server api call
        return of(null)
            .pipe(mergeMap(handleRoute))
            .pipe(materialize()) // call materialize and dematerialize to ensure delay even if an error is thrown (https://github.com/Reactive-Extensions/RxJS/issues/648)
            .pipe(dematerialize());

        function handleRoute() {
            switch (true) {
                case url.endsWith('/api/battery-status') && method === 'GET':
                    return batteryStatus();
                case url.endsWith('/api/ac-status') && method === 'GET':
                    return acStatus();
                case url.endsWith('/api/gsm-number') && method === 'GET':
                    return gsmNumber();
                case url.endsWith('/api/gsm-contact') && method === 'GET':
                    return gsmContact();
                case url.endsWith('/api/gsm-contact') && method === 'POST':
                    return gsmContactSave();
                case url.endsWith('/api/wifi-current') && method === 'GET':
                    return wifiCurrent();
                case url.endsWith('/api/gsm-signal') && method === 'GET':
                    return gsmSignal();
                case url.endsWith('/api/wifi-mac') && method === 'GET':
                    return wifiMac();
                case url.endsWith('/api/notification') && method === 'GET':
                    return notification();
                case url.endsWith('/api/wifi-connect') && method === 'POST':
                    return wifiConnect();
                case url.endsWith('/api/wifi-save') && method === 'POST':
                    return wifiConnect();
                case url.endsWith('/api/wifi-disconnect') && method === 'DELETE':
                    return wifiDisconnect();
                case url.endsWith('/api/wifi-status') && method === 'GET':
                    return wifiStatus();
                case url.endsWith('/api/setup-completed') && method === 'GET':
                    return setupCompleted();
                default:
                    // pass through any requests not handled above
                    return next.handle(request);
            }
        }

        function setupCompleted() {
            return ok({
                "status": 1
            });
        }

        function wifiDisconnect() {
            currentWifi = {};

            return ok({
                "status": "ok"
            });
        }

        function wifiStatus() {
            let isConnected = knownWifi.filter((el) => {
                return el.ssid == currentWifi.ssid && el.password == currentWifi.password
            });

            if (isConnected.length > 0) {
                return ok({
                    "status": 3
                });
            }

            return ok({
                "status": Math.round(Math.random()) ? 1 : 4
            });
        }

        function wifiConnect() {
            console.log(body);
            let wifi = { ssid: "", password: "" };
            wifi.ssid = body.get("ssid");
            wifi.password = body.get("password");
            currentWifi = wifi;
            console.log(currentWifi);

            return ok({
                "status": "ok"
            });
        }

        function notification() {
            return ok({
                entries: [
                    {
                        type: 0,
                        date: "11:30 24/01/2020"
                    },
                    {
                        type: 1,
                        date: "11:30 24/01/2020"
                    },
                    {
                        type: 2,
                        date: "11:30 24/01/2020"
                    },
                    {
                        type: 0,
                        date: "11:42 24/01/2020"
                    }
                ]
            });
        }

        function wifiMac() {
            return ok({
                mac: "0A:CA:F5:C3:4A:D5"
            });
        }

        // route functions
        function gsmSignal() {
            return ok({
                rssi: -34
            });
        }

        function wifiCurrent() {
            return ok({
                ssid: "UPCFREE1337"
            });
        }

        function gsmContact() {
            return ok({
                name: "Tomek",
                number: "0048233433232"
            });
        }

        function gsmContactSave() {
            return ok({
                status: "ok"
            });
        }

        function gsmNumber() {
            return ok({
                number: "0048633424720"
            });
        }

        function acStatus() {
            return ok({
                status: Math.round(Math.random())
            });
        }

        function batteryStatus() {
            return ok({
                status: (Math.round((Math.random() * 600)) + 3600)
            });
        }

        function authenticate() {
            const { username, password } = body;
            const user = users.find(x => x.username === username && x.password === password);
            if (!user) return error('Username or password is incorrect');
            return ok({
                id: user.id,
                username: user.username,
                firstName: user.firstName,
                lastName: user.lastName,
                token: 'fake-jwt-token'
            })
        }

        function register() {
            const user = body

            if (users.find(x => x.username === user.username)) {
                return error('Username "' + user.username + '" is already taken')
            }

            user.id = users.length ? Math.max(...users.map(x => x.id)) + 1 : 1;
            users.push(user);
            localStorage.setItem('users', JSON.stringify(users));

            return ok();
        }

        function getUsers() {
            if (!isLoggedIn()) return unauthorized();
            return ok(users);
        }

        function deleteUser() {
            if (!isLoggedIn()) return unauthorized();

            users = users.filter(x => x.id !== idFromUrl());
            localStorage.setItem('users', JSON.stringify(users));
            return ok();
        }

        // helper functions

        function ok(body?) {
            return of(new HttpResponse({ status: 200, body }))
        }

        function error(message) {
            return throwError({ error: { message } });
        }

        function unauthorized() {
            return throwError({ status: 401, error: { message: 'Unauthorised' } });
        }

        function isLoggedIn() {
            return headers.get('Authorization') === 'Bearer fake-jwt-token';
        }

        function idFromUrl() {
            const urlParts = url.split('/');
            return parseInt(urlParts[urlParts.length - 1]);
        }
    }
}

export const fakeBackendProvider = {
    // use fake backend in place of Http service for backend-less development
    provide: HTTP_INTERCEPTORS,
    useClass: FakeBackendInterceptor,
    multi: true
};