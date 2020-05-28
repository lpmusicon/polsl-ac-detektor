import { Component, OnInit } from '@angular/core';
import { FormBuilder, FormGroup, Validators } from '@angular/forms';
import { Router } from '@angular/router';
import { ApiService } from '../api.service';

@Component({
  selector: 'app-setup-gsm',
  templateUrl: './setup-gsm.component.html',
  styleUrls: ['./setup-gsm.component.scss']
})
export class SetupGsmComponent implements OnInit {

  public setup: FormGroup;

  constructor(private fb: FormBuilder, private router: Router, private api: ApiService) { }

  public ngOnInit(): void {
    this.setup = this.fb.group({
      name: ["", Validators.compose([Validators.required, Validators.minLength(3), Validators.maxLength(15)])],
      phone: ["", Validators.compose([Validators.required, Validators.minLength(8), Validators.maxLength(15), Validators.pattern('^[0-9]*$')])]
    });
  }

  public onSubmit() {
    if(!this.setup.valid) {
      console.warn("Invalid form", this.setup);
      return;
    }

    this.setup.disable();

    this.api.gsmContactSave(this.setup.value).subscribe({
      next: (status) => {
        console.log("Save GSM status: ", status);
        this.router.navigate(["/setup-finished"]);
      }
    });
  }

}
