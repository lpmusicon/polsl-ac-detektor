import { Component, OnInit } from '@angular/core';
import { ApiService } from '../api.service';
import { FormGroup, FormBuilder, Validators } from '@angular/forms';
import { Router } from '@angular/router';

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

  public formGsm: FormGroup;
  public formWifi: FormGroup;

  constructor(private fb: FormBuilder, private router: Router, private api: ApiService) { }

  ngOnInit(): void {
    this.formGsm = this.fb.group({
      name: ["", Validators.compose([Validators.required, Validators.minLength(3), Validators.maxLength(15)])],
      phone: ["", Validators.compose([Validators.required, Validators.minLength(8), Validators.maxLength(15), Validators.pattern('^[0-9]*$')])]
    });

    this.formGsm.disable();

    this.formWifi = this.fb.group({
      ssid: ["", Validators.compose([Validators.required, Validators.minLength(1), Validators.maxLength(31)])],
      password: ["", Validators.compose([Validators.required, Validators.minLength(8), Validators.maxLength(63)])]
    });

    this.formWifi.disable();

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
        this.gsm.rssi = `${data.status} dBm`;
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
