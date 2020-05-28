import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class ApiService {

  private apiPrefix = "http://192.168.4.1";

  constructor(private http: HttpClient) {
    this.refreshSetup();
  }

  public refreshSetup(): void {
    this.setupCompleted().subscribe({
      next: (data) => {
        this.isSetupCompleted = data.status == 1;
        console.log("Updated setup status", this.isSetupCompleted);
      }
    });
  }

  public isSetupCompleted = false;

  public setupCompleted(): Observable<any> {
    return this.http.get(this.apiPrefix + "/api/setup-completed");
  }

  public batteryStatus(): Observable<any> {
    return this.http.get(this.apiPrefix + "/api/battery-status");
  }

  public acStatus(): Observable<any> {
    return this.http.get(this.apiPrefix + "/api/ac-status");
  }

  public gsmSignal(): Observable<any> {
    return this.http.get(this.apiPrefix + "/api/gsm-signal");
  }

  public gsmNumber(): Observable<any> {
    return this.http.get(this.apiPrefix + "/api/gsm-number");
  }

  public gsmContact(): Observable<any> {
    return this.http.get(this.apiPrefix + "/api/gsm-contact");
  }

  public gsmContactSave(contact): Observable<any> {
    const formData = new FormData();
    formData.append('name', contact.name);
    formData.append('number', contact.phone);
    return this.http.post(this.apiPrefix + "/api/gsm-contact", formData);
  }

  public wifiStatus(): Observable<any> {
    return this.http.get(this.apiPrefix + "/api/wifi-status");
  }

  public wifiMAC(): Observable<any> {
    return this.http.get(this.apiPrefix + "/api/wifi-mac");
  }

  public wifiDisconnect(): Observable<any> {
    return this.http.delete(this.apiPrefix + "/api/wifi-disconnect", {});
  }

  public wifiConnect(setup): Observable<any> {
    const formData = new FormData();
    formData.append('ssid', setup.ssid);
    formData.append('password', setup.password);
    return this.http.post(this.apiPrefix + "/api/wifi-connect", formData);
  }

  public wifiSave(setup): Observable<any> {
    const formData = new FormData();
    formData.append('ssid', setup.ssid);
    formData.append('password', setup.password);
    return this.http.post(this.apiPrefix + "/api/wifi-save", formData);
  }

  public wifiCurrent(): Observable<any> {
    return this.http.get(this.apiPrefix + "/api/wifi-current");
  }

  public notification(): Observable<any> {
    return this.http.get(this.apiPrefix + "/api/notification");
  }
}
