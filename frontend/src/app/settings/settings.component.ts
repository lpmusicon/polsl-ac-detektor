import { Component, OnInit } from '@angular/core';
import { ApiService } from '../api.service';

@Component({
  selector: 'app-settings',
  templateUrl: './settings.component.html',
  styleUrls: ['./settings.component.scss']
})
export class SettingsComponent implements OnInit {

  public gsm = {
    name: "",
    phone: "",
    rssi: "",
    cardNumber: ""
  }

  public wifi = {
    ssid: "",
    mac: ""
  }

  public battery = {
    voltage: "",
    charging: 0
  };

  constructor(private api: ApiService) { }

  ngOnInit(): void {
    this.api.batteryStatus().subscribe({
      next: (data) => {
        this.battery.voltage = `${data.status/1000}V`;
      }
    });

    this.api.acStatus().subscribe({
      next: (data) => {
        this.battery.charging = data.status
      }
    });

    this.api.gsmSignal().subscribe({
      next: (data) => {
        this.gsm.rssi = `${data.rssi} dBm`;
      }
    });

    this.api.gsmContact().subscribe({
      next: (data) => {
        this.gsm.name = data.name;
        this.gsm.phone = data.number;
      }
    });

    this.api.gsmNumber().subscribe({
      next: (data) => {
        this.gsm.cardNumber = data.number;
      }
    });

    this.api.wifiMAC().subscribe({
      next: (data) => {
        this.wifi.mac = data.mac;
      }
    });

    this.api.wifiCurrent().subscribe({
      next: (data) => {
        this.wifi.ssid = data.ssid;
      }
    });
  }

}
