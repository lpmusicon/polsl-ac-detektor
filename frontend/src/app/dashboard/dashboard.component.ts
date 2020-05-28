import { Component, OnInit } from '@angular/core';
import { ApiService } from '../api.service';
import { Subscription, timer } from 'rxjs';
import { switchMap } from 'rxjs/operators';

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.scss']
})
export class DashboardComponent implements OnInit {

  public entries = [];

  private refreshSub: Subscription;

  constructor(private api: ApiService) { }

  ngOnInit(): void {
    this.refreshSub = timer(0, 10000).pipe(switchMap(() => this.api.notification())
    ).subscribe({ next: (data) => { this.entries = data.entries; } });
  }
}
