import { Injectable } from '@angular/core';
import { CanActivate, ActivatedRouteSnapshot, RouterStateSnapshot, UrlTree, Router } from '@angular/router';
import { Observable, of } from 'rxjs';
import { ApiService } from './api.service';
import { map } from 'rxjs/internal/operators/map';

@Injectable({
  providedIn: 'root'
})
export class SetupCompletedGuard implements CanActivate {

  constructor(private api: ApiService, private router: Router) { }

  canActivate(
    next: ActivatedRouteSnapshot,
    state: RouterStateSnapshot): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {

    return this.api.setupCompleted().pipe(map(
      (data) => {
        console.log(data);

        if(data.status != 1) {
          this.router.navigateByUrl('setup-wifi');
          return false;
        }

        return true;
      }
    ));
  }

}
