import { Icon } from "../icon";

export const IconLink = ({ href, src, alt }) => (
  <a href={href}>
    <Icon src={src} alt={alt} />
  </a>
);
