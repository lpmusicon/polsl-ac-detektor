import { Component, OnInit } from '@angular/core';
import { ApiService } from '../api.service';
import { Router } from '@angular/router';

@Component({
  selector: 'app-setup-finished',
  templateUrl: './setup-finished.component.html',
  styleUrls: ['./setup-finished.component.scss']
})
export class SetupFinishedComponent implements OnInit {

  constructor(private api: ApiService, private router: Router) { }

  ngOnInit(): void {
    this.api.refreshSetup();

    setTimeout(() => {
      this.router.navigateByUrl('/');
    }, 5000);
  }

}
