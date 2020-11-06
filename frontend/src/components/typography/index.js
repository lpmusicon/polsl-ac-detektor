import styled from "styled-components";

const H1 = styled.h1`
  display: block;
  font-family: "Montserrat", Arial, Helvetica, sans-serif;
  font-size: 48px;
  font-weight: 400;
  color: #becbd8;
  padding: 0;
  margin: 16px 0 8px;
`;

const H2 = styled.h2`
  display: block;
  font-family: "Montserrat", Arial, Helvetica, sans-serif;
  font-size: 14px;
  font-weight: 600;
  color: #becbd8;
  padding: 0;
  margin: 8px 0 8px;
  ${(p) => (p.withMargin ? "margin: 8px 0 16px;" : null)}
`;

const Body1 = styled.p`
  display: block;
  font-family: "Montserrat", Arial, Helvetica, sans-serif;
  font-size: 14px;
  font-weight: 400;
  color: #becbd8;
  padding: 0;
  margin: 8px 0 8px;
`;

const Body2 = styled.p`
  display: block;
  font-family: "Montserrat", Arial, Helvetica, sans-serif;
  font-size: 18px;
  font-weight: 400;
  color: #becbd8;
  padding: 0;
  margin: ${(p) => (p.withMargin ? "8px 0 12px" : 0)};
`;

const Accent = styled.p`
  display: block;
  font-family: "Montserrat", Arial, Helvetica, sans-serif;
  font-size: 14px;
  font-weight: 400;
  color: #44d194;
  padding: 0;
  margin: ${(p) => (p.withMargin ? "8px 0 8px" : 0)};
`;

export const Variant = {
  h1: "h1",
  h2: "h2",
  body1: "body1",
  body2: "body2",
  accent: "accent",
};

export const Typography = ({ variant, text, ...props }) => {
  switch (variant) {
    case Variant.h1:
      return <H1 {...props}>{text}</H1>;
    case Variant.h2:
      return <H2 {...props}>{text}</H2>;
    case Variant.body1:
      return <Body1 {...props}>{text}</Body1>;
    case Variant.body2:
      return <Body2 {...props}>{text}</Body2>;
    case Variant.accent:
      return <Accent {...props}>{text}</Accent>;
    default:
      return <Body1 {...props}>{text}</Body1>;
  }
};
