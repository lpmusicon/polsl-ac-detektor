import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { DashboardComponent } from './dashboard/dashboard.component';
import { SetupWifiComponent } from './setup-wifi/setup-wifi.component';
import { SetupGsmComponent } from './setup-gsm/setup-gsm.component';
import { SetupFinishedComponent } from './setup-finished/setup-finished.component';
import { SettingsComponent } from './settings/settings.component';
import { SetupMqttComponent } from './setup-mqtt/setup-mqtt.component';
import { SetupCompletedGuard } from './setup-completed.guard';
import { SetupNotCompletedGuard } from './setup-not-completed.guard';


const routes: Routes = [
  { path: 'dashboard', component: DashboardComponent, canActivate: [SetupCompletedGuard] },
  { path: 'settings', component: SettingsComponent, canActivate: [SetupCompletedGuard] },
  { path: 'setup-wifi', component: SetupWifiComponent, canActivate: [SetupNotCompletedGuard] },
  { path: 'setup-gsm', component: SetupGsmComponent, canActivate: [SetupNotCompletedGuard] },
  { path: 'setup-finished', component: SetupFinishedComponent, canActivate: [SetupNotCompletedGuard] },
  { path: '', redirectTo: 'dashboard', pathMatch: 'full'},
  { path: '**', redirectTo: 'dashboard', pathMatch: 'full'}
];

@NgModule({
  imports: [RouterModule.forRoot(routes, {useHash: true})],
  exports: [RouterModule]
})
export class AppRoutingModule { }
