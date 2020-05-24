import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class ApiService {

  constructor(private http: HttpClient) { }

  public batteryStatus(): Observable<any> {
    return this.http.get("/api/battery-status");
  }

  public acStatus(): Observable<any> {
    return this.http.get("/api/ac-status");
  }

  public gsmSignal(): Observable<any> {
    return this.http.get("/api/gsm-signal");
  }

  public gsmNumber(): Observable<any> {
    return this.http.get("/api/gsm-number");
  }

  public gsmContact(): Observable<any> {
    return this.http.get("/api/gsm-contact");
  }

  public wifiStatus(): Observable<any> {
    return this.http.get("/api/wifi-status");
  }

  public wifiMAC(): Observable<any> {
    return this.http.get("/api/wifi-mac");
  }

  public wifiConnect(ssid, password): Observable<any> {
    return this.http.post("/api/wifi-connect", { ssid: ssid, password: password });
  }

  public wifiCurrent(): Observable<any> {
    return this.http.get("/api/wifi-current");
  }

  public notification(): Observable<any> {
    return this.http.get("/api/notification");
  }
}
