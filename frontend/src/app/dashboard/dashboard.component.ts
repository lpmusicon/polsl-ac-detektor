import { Component, OnInit } from '@angular/core';
import { ApiService } from '../api.service';

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.scss']
})
export class DashboardComponent implements OnInit {

  public entries = [];  

  constructor(private api: ApiService) { }

  ngOnInit(): void {
    this.api.notification().subscribe({
      next: (data) => {
       this.entries = data.entries;
      }
    })
  }

}
