import { Component, OnInit, OnDestroy } from '@angular/core';
import { FormBuilder, FormGroup, Validators } from '@angular/forms';
import { Router } from '@angular/router';
import { ApiService } from '../api.service';
import { Subscription } from 'rxjs/internal/Subscription';
import { switchMap } from 'rxjs/operators';
import { timer } from 'rxjs/index';

@Component({
  selector: 'app-setup-wifi',
  templateUrl: './setup-wifi.component.html',
  styleUrls: ['./setup-wifi.component.scss']
})
export class SetupWifiComponent implements OnInit, OnDestroy {

  public setup: FormGroup;
  public wifi;
  private wifiStatus: Subscription;

  constructor(private fb: FormBuilder, private router: Router, private api: ApiService) { }

  public ngOnInit(): void {
    this.setup = this.fb.group({
      ssid: ["", Validators.compose([Validators.required, Validators.minLength(1), Validators.maxLength(31)])],
      password: ["", Validators.compose([Validators.required, Validators.minLength(8), Validators.maxLength(63)])]
    });
  }

  public ngOnDestroy(): void {
    this.wifiStatus.unsubscribe();
  }

  public onSubmit() {
    if(!this.setup.valid) {
      console.warn("Invalid form", this.setup);
      return;
    }

    console.log(this.setup.value);
    this.api.wifiConnect(this.setup.value).subscribe({
      next: (response) => {
        console.log('ask for status');
        this.wifiStatus = timer(0, 1000).pipe(switchMap(() => this.api.wifiStatus())
        ).subscribe({next: this.transformStatus.bind(this)});
      }
    })

    console.log(this.setup);
  }

  private transformStatus(data) {
    console.log(data);
    this.wifi = data.status;
    if(data.status == 3) {
      this.router.navigate(["/setup-gsm"]);
    }
  }

}
