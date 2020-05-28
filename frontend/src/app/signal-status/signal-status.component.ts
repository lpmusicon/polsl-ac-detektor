import { Component, OnInit, OnDestroy } from '@angular/core';
import { ApiService } from '../api.service';
import { Subscription } from 'rxjs/internal/Subscription';
import { switchMap } from 'rxjs/operators';
import { timer } from 'rxjs/index';

@Component({
  selector: 'app-signal-status',
  templateUrl: './signal-status.component.html',
  styleUrls: ['./signal-status.component.scss']
})
export class SignalStatusComponent implements OnInit, OnDestroy {

  public batteryPercentage: number;
  public statusIcon: string;

  private sub: Subscription;

  constructor(private api: ApiService) { }

  ngOnInit(): void {
    this.statusIcon = "assets/battery-charging.svg";
    this.batteryPercentage = 50;

    this.sub = timer(0, 10000).pipe(switchMap(() => this.api.batteryStatus())).subscribe({next: this.transformStatus.bind(this)});
  }

  private transformStatus(data) {
    const minV = 3600;
    const maxV = 4200;

    this.batteryPercentage = Math.ceil(((data.status - minV) / (maxV - minV)) * 100);

    if(this.batteryPercentage > 75) {
      this.statusIcon = "assets/battery-full.svg";
    } else if(this.batteryPercentage > 40) {
      this.statusIcon = "assets/battery.svg";
    } else {
      this.statusIcon = "assets/battery-low.svg";
    }
  }

  ngOnDestroy() {
    this.sub.unsubscribe();
  }

}
