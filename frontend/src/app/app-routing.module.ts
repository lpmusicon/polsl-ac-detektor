import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { DashboardComponent } from './dashboard/dashboard.component';
import { SetupWifiComponent } from './setup-wifi/setup-wifi.component';
import { SetupGsmComponent } from './setup-gsm/setup-gsm.component';
import { SetupFinishedComponent } from './setup-finished/setup-finished.component';
import { SettingsComponent } from './settings/settings.component';
import { SetupMqttComponent } from './setup-mqtt/setup-mqtt.component';


const routes: Routes = [
  { path: 'dashboard', component: DashboardComponent },
  { path: 'settings', component: SettingsComponent },
  { path: 'setup-mqtt', component: SetupMqttComponent },
  { path: 'setup-wifi', component: SetupWifiComponent },
  { path: 'setup-gsm', component: SetupGsmComponent },
  { path: 'setup-finished', component: SetupFinishedComponent },
  { path: '', redirectTo: 'dashboard', pathMatch: 'full'},
  { path: '*', redirectTo: 'dashboard', pathMatch: 'full'}
];

@NgModule({
  imports: [RouterModule.forRoot(routes, {useHash: true})],
  exports: [RouterModule]
})
export class AppRoutingModule { }
