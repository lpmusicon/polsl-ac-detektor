import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { NotificationsComponent } from './notifications/notifications.component';
import { SettingsComponent } from './settings/settings.component';
import { LoadingComponent } from './loading/loading.component';
import { SetupWifiComponent } from './setup-wifi/setup-wifi.component';
import { SetupMqttComponent } from './setup-mqtt/setup-mqtt.component';
import { DashboardComponent } from './dashboard/dashboard.component';

@NgModule({
  declarations: [
    AppComponent,
    NotificationsComponent,
    SettingsComponent,
    LoadingComponent,
    SetupWifiComponent,
    SetupMqttComponent,
    DashboardComponent
  ],
  imports: [
    BrowserModule,
    AppRoutingModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
